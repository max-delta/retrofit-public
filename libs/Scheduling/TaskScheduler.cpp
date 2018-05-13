#include "stdafx.h"
#include "TaskScheduler.h"

#include "Scheduling/taskpools/TaskPool.h"
#include "Scheduling/taskworkers/TaskWorker.h"

#include "core/ptr/ptr_transform.h"

#include "rftl/thread"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

TaskScheduler::RegisteredWorker::RegisteredWorker( SharedPtr<TaskWorker> const & worker )
	: mWorker( worker )
{
	//
}



TaskScheduler::RegisteredWorker::RegisteredWorker( RegisteredWorker && rhs )
	: mWorker( rftl::move( rhs.mWorker ) )
	, mActiveTaskID( rftl::move( rhs.mActiveTaskID ) )
	, mActiveTask( rftl::move( rhs.mActiveTask ) )
	, mActivePool( rftl::move( rhs.mActivePool ) )
	, mFlaggedAsActive( rhs.mFlaggedAsActive.load( rftl::memory_order::memory_order_acquire ) )
	, mFlaggedForUnregistration( rhs.mFlaggedForUnregistration.load( rftl::memory_order::memory_order_acquire ) )
{
	//
}



TaskScheduler::RegisteredWorker & TaskScheduler::RegisteredWorker::operator=( RegisteredWorker && rhs )
{
	if( this != &rhs )
	{
		this->mWorker = rftl::move( rhs.mWorker );
		this->mActiveTaskID = rftl::move( rhs.mActiveTaskID );
		this->mActiveTask = rftl::move( rhs.mActiveTask );
		this->mActivePool = rftl::move( rhs.mActivePool );
		this->mFlaggedAsActive.store( rhs.mFlaggedAsActive.load( rftl::memory_order::memory_order_acquire ), rftl::memory_order::memory_order_release );
		this->mFlaggedForUnregistration.store( rhs.mFlaggedForUnregistration.load( rftl::memory_order::memory_order_acquire ), rftl::memory_order::memory_order_release );
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

TaskScheduler::RegisteredPool::RegisteredPool( UniquePtr<TaskPool>&& pool )
	: mPool( rftl::move( pool ) )
{
	//
}



TaskScheduler::RegisteredPool::RegisteredPool( RegisteredPool && rhs )
	: mPool( rftl::move( rhs.mPool ) )
{
	//
}



TaskScheduler::RegisteredPool & TaskScheduler::RegisteredPool::operator=( RegisteredPool && rhs )
{
	if( this != &rhs )
	{
		this->mPool = rftl::move( rhs.mPool );
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

TaskScheduler::TaskScheduler()
{
	static_assert( kNumPriorities == 4, "Unexpected value" );
	mPrioritizedPools[0].mPriority = TaskPriority::RealTime;
	mPrioritizedPools[1].mPriority = TaskPriority::High;
	mPrioritizedPools[2].mPriority = TaskPriority::Normal;
	mPrioritizedPools[3].mPriority = TaskPriority::Idle;
}



WeakSharedPtr<TaskWorker> TaskScheduler::RegisterWorker( UniquePtr<TaskWorker>&& worker )
{
	RF_ASSERT( worker != nullptr );

	// Transform to shared
	SharedPtr<TaskWorker> sharedWorker;
	PtrTransformer<TaskWorker>::PerformTransformation( rftl::move( worker ), sharedWorker );

	// Store worker
	{
		WriterLock lock{ mWorkerLock };
		mWorkers.emplace_back( sharedWorker );
	}

	// Attempt dispatching
	AttemptDispatch();

	return sharedWorker;
}



void TaskScheduler::UnregisterWorker( WeakPtr<TaskWorker> const & worker )
{
	RF_ASSERT( worker != nullptr );
	TaskWorker const* workerKey = worker;
	RF_ASSERT( workerKey != nullptr );

	// Find worker
	{
		WriterLock lock{ mWorkerLock };
		for( WorkerList::iterator iter = mWorkers.begin(); iter != mWorkers.end(); iter++ )
		{
			RegisteredWorker& registration = *iter;
			if( registration.mWorker != workerKey )
			{
				continue;
			}

			// Flag for removal
			registration.mFlaggedForUnregistration.store( true, rftl::memory_order::memory_order_release );

			// Attempt to block dispatching
			bool const wasActive = registration.mFlaggedAsActive.exchange( true, rftl::memory_order::memory_order_acq_rel );
			if( wasActive )
			{
				// In use, will need to wait to remove
				return;
			}

			// Remove
			RF_ASSERT( registration.mActiveTaskID == kInvalidTaskID );
			RF_ASSERT( registration.mActiveTask == nullptr );
			RF_ASSERT( registration.mActivePool == nullptr );
			mWorkers.erase( iter );
			return;
		}
	}

	RF_DBGFAIL_MSG( "Failed to locate worker" );
}



WeakPtr<TaskPool> TaskScheduler::RegisterPool( UniquePtr<TaskPool>&& pool, TaskPriority priority )
{
	RF_ASSERT( pool != nullptr );
	WeakPtr<TaskPool> retVal = pool;

	// Select priority block
	PriorityBlock& priorityBlock = GetPriorityBlock( priority );

	// Store pool
	{
		WriterLock lock{ mPoolLock };
		priorityBlock.mPools.emplace_back( rftl::move( pool ) );
	}

	// Attempt dispatching
	AttemptDispatch();

	return retVal;
}



void TaskScheduler::UnregisterPool( WeakPtr<TaskPool> const & pool )
{
	RF_ASSERT( pool != nullptr );
	TaskPool const* poolKey = pool;
	RF_ASSERT( poolKey != nullptr );

	for( PriorityBlock& priorityBlock : mPrioritizedPools )
	{
		ReaderLock lock{ mPoolLock };
		for( PoolList::const_iterator iter = priorityBlock.mPools.begin(); iter != priorityBlock.mPools.end(); iter++ )
		{
			if( iter->mPool == poolKey )
			{
				priorityBlock.mPools.erase( iter );
				return;
			}
		}
	}

	RF_DBGFAIL_MSG( "Failed to locate pool" );
}



void TaskScheduler::StartDispatching()
{
	RF_ASSERT( mDispatchingPermitted.load( rftl::memory_order::memory_order_acquire ) == false );
	mDispatchingPermitted.store( true, rftl::memory_order::memory_order_release );

	AttemptDispatch();
}



void TaskScheduler::StopDispatching( bool blockUntilWorkersIdle )
{
	RF_ASSERT( mDispatchingPermitted.load( rftl::memory_order::memory_order_acquire ) == true );
	mDispatchingPermitted.store( false, rftl::memory_order::memory_order_release );

	if( blockUntilWorkersIdle )
	{
		// Block until workers idle
		while( true )
		{
			// This should only happen if someone starts dispatching while
			//  we're trying to stop, which means we can't honor the request to
			//  block properly
			RF_ASSERT( mDispatchingPermitted.load( rftl::memory_order::memory_order_acquire ) == false );

			if( AreAllWorkersIdle() )
			{
				return;
			}

			// Don't burn the house down
			rftl::this_thread::yield();
		}
	}
}



bool TaskScheduler::AllTasksAreCurrentlyCompleted() const
{
	RF_ASSERT_MSG(
		mDispatchingPermitted.load( rftl::memory_order::memory_order_acquire ) == false,
		"It is meaninglessly dangerous to check worker state when workers can"
		" still receive newly dispatched work" );

	// Check all workers
	if( AreAllWorkersIdle() == false )
	{
		return false;
	}

	// Check all pools
	{
		for( PriorityBlock const& priorityBlock : mPrioritizedPools )
		{
			ReaderLock lock{ mPoolLock };
			for( RegisteredPool const& registeredPool : priorityBlock.mPools )
			{
				// NOTE: Sketchy non-const access
				TaskPool* pool = registeredPool.mPool;

				bool empty = true;
				{
					// Fetch and return === peek
					TaskID taskID = kInvalidTaskID;
					Task* unused = nullptr;
					pool->FetchAndBlockNextUnblockedTask( unused, taskID );
					if( taskID != kInvalidTaskID )
					{
						empty = false;
						pool->ReturnAndUnblockTask( taskID, TaskState::Ready );
					}
				}

				if( empty == false )
				{
					return false;
				}
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

void TaskScheduler::OnWorkComplete( TaskWorker * worker, Task * task, TaskState newState )
{
	RF_ASSERT( worker != nullptr );
	RF_ASSERT( task != nullptr );
	RF_ASSERT( newState != TaskState::Invalid );

	bool shouldRemove = false;

	// Update worker
	{
		ReaderLock workerLock{ mWorkerLock };

		// Find worker
		WorkerList::iterator foundWorker = mWorkers.end();
		{
			for( WorkerList::iterator iter = mWorkers.begin(); iter != mWorkers.end(); iter++ )
			{
				RegisteredWorker& registration = *iter;
				if( registration.mWorker == worker )
				{
					foundWorker = iter;
					break;
				}
			}
		}
		RF_ASSERT( foundWorker != mWorkers.end() );
		RegisteredWorker& registration = *foundWorker;
		RF_ASSERT( registration.mActiveTask == task );
		RF_ASSERT( registration.mActiveTaskID != kInvalidTaskID );
		RF_ASSERT( registration.mFlaggedAsActive.load( rftl::memory_order::memory_order_acquire ) );

		TaskID const taskID = registration.mActiveTaskID;
		WeakPtr<TaskPool> weakPool = registration.mActivePool;
		registration.mActiveTaskID = kInvalidTaskID;
		registration.mActiveTask = nullptr;
		registration.mActivePool = nullptr;

		// Update pool
		{
			ReaderLock poolLock{ mPoolLock };

			// NOTE: Weak, may have been unregistered
			TaskPool* pool = weakPool;
			if( pool != nullptr )
			{
				if( newState == TaskState::Ready )
				{
					pool->ReturnAndUnblockTask( taskID, newState );
				}
				else if( newState == TaskState::Terminate )
				{
					pool->RemoveBlockedTask( taskID, newState );
				}
				else
				{
					RF_DBGFAIL_MSG( "Invalid state" );
				}
			}
		}

		if( registration.mFlaggedForUnregistration.load( rftl::memory_order::memory_order_acquire ) )
		{
			shouldRemove = true;
		}
		else
		{
			// Clear worker for more work
			registration.mFlaggedAsActive.store( false, rftl::memory_order::memory_order_release );
		}
	}

	if( shouldRemove )
	{
		// TODO: Remove
		RF_DBGFAIL_MSG( "TODO" );
	}

	AttemptDispatch();
}



void TaskScheduler::OnTasksNewlyAvailable( TaskPool * pool )
{
	AttemptDispatch();
}

///////////////////////////////////////////////////////////////////////////////

void TaskScheduler::AttemptDispatch()
{
	if( mDispatchingPermitted.load( rftl::memory_order::memory_order_acquire ) == false )
	{
		// Dispatching currently disabled
		return;
	}

	ReaderLock lock{ mWorkerLock };
	for( RegisteredWorker& worker : mWorkers )
	{
		if( worker.mFlaggedForUnregistration.load( rftl::memory_order::memory_order_acquire ) )
		{
			continue;
		}

		// Flag as active before dispatching
		bool const wasActive = worker.mFlaggedAsActive.exchange( true, rftl::memory_order::memory_order_acq_rel );
		if( wasActive )
		{
			continue;
		}

		bool const dispatched = AttemptDispatch( worker );
		if( dispatched == false )
		{
			// Failed to actually dispatch, undo active flagging
			worker.mFlaggedAsActive.store( false, rftl::memory_order::memory_order_release );
		}
	}
}



bool TaskScheduler::AttemptDispatch( RegisteredWorker& worker )
{
	RF_ASSERT( worker.mFlaggedAsActive.load( rftl::memory_order::memory_order_acquire ) );
	RF_ASSERT( worker.mActiveTaskID == kInvalidTaskID );
	RF_ASSERT( worker.mActiveTask == nullptr );
	RF_ASSERT( worker.mActivePool == nullptr );
	RF_ASSERT( worker.mWorker != nullptr );

	// Loop through priority blocks
	// NOTE: Earliest blocks are highest priority
	for( PriorityBlock& priorityBlock : mPrioritizedPools )
	{
		ReaderLock lock{ mPoolLock };

		// Loop through each pool, starting with next round-robin selection
		size_t const numPools = priorityBlock.mPools.size();
		if( numPools == 0 )
		{
			continue;
		}
		size_t const startIndex = priorityBlock.mLastDispatchIndex.load( rftl::memory_order::memory_order_acquire ) + 1;
		size_t index = startIndex % numPools;
		for( size_t loopCount = 0; loopCount < numPools; loopCount++ )
		{
			RegisteredPool& registration = priorityBlock.mPools.at( index );
			WeakPtr<TaskPool> weakPool = registration.mPool;
			TaskPool* pool = weakPool;

			// Attempt to fetch
			Task* task = nullptr;
			TaskID taskID = kInvalidTaskID;
			pool->FetchAndBlockNextUnblockedTask( task, taskID );
			if( taskID != kInvalidTaskID )
			{
				RF_ASSERT( task != nullptr );

				// Mark the dispatch
				// NOTE: Race condition with other dispatching threads, but
				//  don't really care
				priorityBlock.mLastDispatchIndex.store( index, rftl::memory_order::memory_order_release );

				// Push work to worker and return
				worker.mActivePool = weakPool;
				worker.mActiveTask = task;
				worker.mActiveTaskID = taskID;
				worker.mWorker->AddTask( task, this );
				return true;
			}
			else
			{
				RF_ASSERT( task == nullptr );
			}

			// Try next pool in round-robin
			index = ( index + 1 ) % numPools;
		}
	}

	return false;
}



bool TaskScheduler::AreAllWorkersIdle() const
{
	ReaderLock lock{ mWorkerLock };
	for( RegisteredWorker const& worker : mWorkers )
	{
		if( worker.mFlaggedAsActive.load( rftl::memory_order::memory_order_acquire ) )
		{
			return false;
		}
	}

	return true;
}



TaskScheduler::PriorityBlock & TaskScheduler::GetPriorityBlock( TaskPriority priority )
{
	static_assert( kNumPriorities == 4, "Unexpected value" );
	switch( priority )
	{
		case RF::scheduling::TaskPriority::RealTime:
			return mPrioritizedPools[0];
		case RF::scheduling::TaskPriority::High:
			return mPrioritizedPools[1];
		case RF::scheduling::TaskPriority::Normal:
			return mPrioritizedPools[2];
		case RF::scheduling::TaskPriority::Idle:
			return mPrioritizedPools[3];

		case RF::scheduling::TaskPriority::Invalid:
		case RF::scheduling::TaskPriority::MaxVal:
		default:
			RF_DBGFAIL();
			rftl::abort();
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
