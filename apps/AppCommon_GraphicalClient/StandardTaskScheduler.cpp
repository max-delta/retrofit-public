#include "stdafx.h"
#include "StandardTaskScheduler.h"

#include "Scheduling/taskworkers/ThreadableTaskWorker.h"
#include "Scheduling/taskpools/FIFOTaskPool.h"
#include "Scheduling/TaskScheduler.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"

#include "rftl/thread"
#include "rftl/chrono"


namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

StandardTaskScheduler::StandardTaskScheduler( size_t workerThreadCount )
{
	mTaskScheduler = DefaultCreator<scheduling::TaskScheduler>::Create();

	// For each worker thread...
	for( size_t i = 0; i < workerThreadCount; i++ )
	{
		// Allocate
		UniquePtr<ThreadedWorker> newThreadedWorker = DefaultCreator<ThreadedWorker>::Create();

		// Create worker
		UniquePtr<scheduling::ThreadableTaskWorker> newWorker = DefaultCreator<scheduling::ThreadableTaskWorker>::Create();
		newThreadedWorker->mWorkerInterface = newWorker;

		// Register
		WeakSharedPtr<scheduling::TaskWorker> tempHandle = mTaskScheduler->RegisterWorker( rftl::move( newWorker ) );
		newThreadedWorker->mWorkerHandle = tempHandle.Lock();

		// Run worker asyncronously
		RF_ASSERT( newThreadedWorker->mTerminateWorker.load( rftl::memory_order::memory_order_acquire ) == false );
		auto threadedWorker = [](
			scheduling::ThreadableTaskWorker* worker,
			rftl::atomic_bool const* shouldTerminate )->
			void
		{
			size_t sleepAccumulator = 0;
			constexpr size_t kSleepTrigger = 1000;
			while( shouldTerminate->load( rftl::memory_order::memory_order_acquire ) == false )
			{
				size_t const numExecutions = worker->ExecuteUntilStarved();
				if( numExecutions == 0 )
				{
					sleepAccumulator++;
				}
				else
				{
					sleepAccumulator = 0;
				}

				if( sleepAccumulator >= kSleepTrigger )
				{
					// Nothing much happening, put the thread into low-power
					rftl::this_thread::sleep_for( rftl::chrono::milliseconds( 1 ) );
				}
				else
				{
					// Yield to OS briefly
					rftl::this_thread::yield();
				}
			}
		};
		newThreadedWorker->mThreadHandle = rftl::async( rftl::launch::async, threadedWorker, static_cast<scheduling::ThreadableTaskWorker*>( newThreadedWorker->mWorkerInterface ), &( newThreadedWorker->mTerminateWorker ) );

		// Store
		mThreadedWorkers.emplace_back( rftl::move( newThreadedWorker ) );
	}

	// For the main thread...
	{
		// Allocate
		mMainThreadWorker = DefaultCreator<ThreadedWorker>::Create();

		// Create worker
		UniquePtr<scheduling::ThreadableTaskWorker> newWorker = DefaultCreator<scheduling::ThreadableTaskWorker>::Create();
		mMainThreadWorker->mWorkerInterface = newWorker;

		// Register
		WeakSharedPtr<scheduling::TaskWorker> tempHandle = mTaskScheduler->RegisterWorker( rftl::move( newWorker ) );
		mMainThreadWorker->mWorkerHandle = tempHandle.Lock();
	}

	// Add generic priority pools
	{
		mGenericPools[scheduling::TaskPriority::RealTime] =
			mTaskScheduler->RegisterPool(
				EntwinedCreator<scheduling::FIFOTaskPool>::Create(),
				scheduling::TaskPriority::RealTime );
		mGenericPools[scheduling::TaskPriority::High] =
			mTaskScheduler->RegisterPool(
				EntwinedCreator<scheduling::FIFOTaskPool>::Create(),
				scheduling::TaskPriority::High );
		mGenericPools[scheduling::TaskPriority::Normal] =
			mTaskScheduler->RegisterPool(
				EntwinedCreator<scheduling::FIFOTaskPool>::Create(),
				scheduling::TaskPriority::Normal );
		mGenericPools[scheduling::TaskPriority::Idle] =
			mTaskScheduler->RegisterPool(
				EntwinedCreator<scheduling::FIFOTaskPool>::Create(),
				scheduling::TaskPriority::Idle );
	}

	// Start dispatching
	mTaskScheduler->StartDispatching();
}



StandardTaskScheduler::~StandardTaskScheduler()
{
	// Stop dispatching
	mTaskScheduler->StopDispatching( true );

	// Unregister generic pools
	for( GenericPoolInterfaceMap::value_type const& poolPair : mGenericPools )
	{
		mTaskScheduler->UnregisterPool( poolPair.second );
	}

	// Unregister main thread
	{
		mTaskScheduler->UnregisterWorker( mMainThreadWorker->mWorkerHandle );
	}

	// Unregister and terminate all worker threads
	// NOTE: Not waiting, want to trigger all terminations first
	for( UniquePtr<ThreadedWorker>& worker : mThreadedWorkers )
	{
		mTaskScheduler->UnregisterWorker( worker->mWorkerHandle );
		worker->mTerminateWorker.store( true, rftl::memory_order::memory_order_release );
	}

	// Wait for all worker threads
	for( UniquePtr<ThreadedWorker>& worker : mThreadedWorkers )
	{
		worker->mThreadHandle.wait();
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
