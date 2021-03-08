#include "stdafx.h"

#include "FIFOTaskPool.h"

#include "rftl/vector"


namespace RF::scheduling {
///////////////////////////////////////////////////////////////////////////////

FIFOTaskPool::TaskRecord::TaskRecord( TaskID id, TaskPtr&& task )
	: mID( id )
	, mTask( rftl::move( task ) )
{
	//
}



FIFOTaskPool::TaskRecord& FIFOTaskPool::TaskRecord::operator=( TaskRecord&& rhs )
{
	if( this != &rhs )
	{
		this->mID = rftl::move( rhs.mID );
		this->mTask = rftl::move( rhs.mTask );
		this->mBlocked.store( rhs.mBlocked.load( rftl::memory_order::memory_order_acquire ), rftl::memory_order::memory_order_release );
		this->mAbortPending.store( rhs.mAbortPending.load( rftl::memory_order::memory_order_acquire ), rftl::memory_order::memory_order_release );
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

FIFOTaskPool::~FIFOTaskPool()
{
	RF_ASSERT( mTaskRecords.empty() );
}



TaskID FIFOTaskPool::AddTask( TaskPtr&& task )
{
	RF_ASSERT( task != nullptr );

	TaskID const newID = mIDGen.Generate();

	// Add
	bool wasEmpty;
	{
		WriterLock lock{ mTaskRecordLock };
		wasEmpty = mTaskRecords.empty();
		mTaskRecords.emplace_back( newID, rftl::move( task ) );
	}

	if( wasEmpty )
	{
		OnTasksNewlyAvailable();
	}

	return newID;
}



void FIFOTaskPool::AttemptAbortTask( TaskID taskID )
{
	RF_ASSERT( taskID != kInvalidTaskID );

	// Find and attempt to block
	{
		ReaderLock lock{ mTaskRecordLock };
		for( TaskRecord& record : mTaskRecords )
		{
			if( record.mID == taskID )
			{
				bool const wasAlreadyBlocked = record.mBlocked.exchange( true, rftl::memory_order::memory_order_acq_rel );
				if( wasAlreadyBlocked )
				{
					// Failed to abort, will try later
					record.mAbortPending.store( true, rftl::memory_order::memory_order_release );
					return;
				}
			}
		}
	}

	// Was able to block, now remove
	RemoveBlockedTask( taskID, TaskState::Terminate );
}



void FIFOTaskPool::AttemptAbortAllTasks()
{
	// Find all
	rftl::vector<TaskID> taskIDs;
	{
		ReaderLock lock{ mTaskRecordLock };
		taskIDs.reserve( mTaskRecords.size() );
		for( TaskRecord& record : mTaskRecords )
		{
			taskIDs.emplace_back( record.mID );
		}
	}

	// Abort all
	for( TaskID const& taskID : taskIDs )
	{
		AttemptAbortTask( taskID );
	}
}



void FIFOTaskPool::FetchAndBlockNextUnblockedTask( Task*& task, TaskID& taskID )
{
	// Return first unblocked
	{
		ReaderLock lock{ mTaskRecordLock };
		for( TaskRecord& record : mTaskRecords )
		{
			// Skip tasks with pending abortion
			// NOTE: Relaxed memory order, don't particularly care
			if( record.mAbortPending.load( rftl::memory_order::memory_order_relaxed ) )
			{
				continue;
			}

			// Attempt to block
			bool const wasAlreadyBlocked = record.mBlocked.exchange( true, rftl::memory_order::memory_order_acq_rel );
			if( wasAlreadyBlocked )
			{
				continue;
			}

			task = record.mTask;
			taskID = record.mID;
			return;
		}
	}

	// None found
	task = nullptr;
	taskID = kInvalidTaskID;
}



void FIFOTaskPool::ReturnAndUnblockTask( TaskID taskID, TaskState newState )
{
	RF_ASSERT( taskID != kInvalidTaskID );
	RF_ASSERT( newState == TaskState::Ready );

	// Find and unblock
	{
		ReaderLock lock{ mTaskRecordLock };
		for( TaskRecord& record : mTaskRecords )
		{
			if( record.mID == taskID )
			{
				bool const wasBlocked = record.mBlocked.exchange( false, rftl::memory_order::memory_order_acq_rel );
				RF_ASSERT_MSG( wasBlocked, "Double-unblock of task" );
			}
		}
	}
}



void FIFOTaskPool::RemoveBlockedTask( TaskID taskID, TaskState newState )
{
	RF_ASSERT( taskID != kInvalidTaskID );
	RF_ASSERT( newState == TaskState::Terminate );

	// Find and extract
	{
		WriterLock lock{ mTaskRecordLock };
		for( TaskRecordList::iterator iter = mTaskRecords.begin(); iter != mTaskRecords.end(); iter++ )
		{
			TaskRecord& record = *iter;
			if( record.mID == taskID )
			{
				RF_ASSERT_MSG( record.mBlocked.load( rftl::memory_order::memory_order_acquire ), "Attempting to remove a non-blocked task" );

				TaskPtr task = rftl::move( record.mTask );
				mTaskRecords.erase( iter );
				lock.unlock();
				OnTaskRemoved( rftl::move( task ), taskID, newState );
				return;
			}
		}
	}

	RF_DBGFAIL_MSG( "Failed to locate task" );
	return;
}

///////////////////////////////////////////////////////////////////////////////
}
