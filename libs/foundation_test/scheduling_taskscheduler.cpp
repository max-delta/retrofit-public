#include "stdafx.h"

#include "Scheduling/taskworkers/ThreadableTaskWorker.h"
#include "Scheduling/taskpools/FIFOTaskPool.h"
#include "Scheduling/tasks/FunctorTask.h"
#include "Scheduling/TaskScheduler.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"

#include "rftl/atomic"
#include "rftl/future"
#include "rftl/thread"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

namespace details {

static rftl::atomic_uint8_t sU8Val = 0;

void IncU8Val()
{
	sU8Val.fetch_add( 1, rftl::memory_order::memory_order_release );
}

}



TEST( Scheduling, SingleThreadedSchedulerLifetime )
{
	TaskScheduler scheduler;
	details::sU8Val.store( 0, rftl::memory_order::memory_order_release );

	// Register worker
	WeakSharedPtr<TaskWorker> worker;
	WeakPtr<ThreadableTaskWorker> executingWorker;
	{
		UniquePtr<ThreadableTaskWorker> newWorker = DefaultCreator<ThreadableTaskWorker>::Create();
		executingWorker = newWorker;

		worker = scheduler.RegisterWorker( rftl::move( newWorker ) );
		ASSERT_EQ( newWorker, nullptr );
		ASSERT_NE( worker.Weaken(), nullptr );
		ASSERT_EQ( worker.Weaken(), executingWorker );
	}

	// Keep worker alive while potentially executing
	SharedPtr<TaskWorker> strongWorker = worker.Lock();

	// Worker will fail to find work
	ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
	executingWorker->ExecuteUntilStarved();
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 0 );

	// Register pool
	WeakPtr<TaskPool> pool;
	WeakPtr<FIFOTaskPool> derivedPool;
	{
		UniquePtr<FIFOTaskPool> newPool = EntwinedCreator<FIFOTaskPool>::Create();
		derivedPool = newPool;

		pool = scheduler.RegisterPool( rftl::move( newPool ), TaskPriority::Normal );
		ASSERT_EQ( newPool, nullptr );
		ASSERT_NE( pool, nullptr );
		ASSERT_EQ( pool, derivedPool );
	}

	// Worker will fail to find work
	ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
	executingWorker->ExecuteUntilStarved();
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 0 );

	// Add task
	WeakPtr<Task> task;
	TaskID taskID;
	{
		auto myFunctor = CreateFunctorTask( details::IncU8Val );
		using TaskType = decltype( myFunctor );
		UniquePtr<TaskType> newTask = EntwinedCreator<TaskType>::Create( rftl::move( myFunctor ) );
		task = newTask;

		taskID = pool->AddTask( rftl::move( newTask ) );
		ASSERT_EQ( newTask, nullptr );
		ASSERT_NE( task, nullptr );
		ASSERT_NE( taskID, kInvalidTaskID );
	}

	// Worker will fail to find work
	ASSERT_FALSE( scheduler.AllTasksAreCurrentlyCompleted() );
	executingWorker->ExecuteUntilStarved();
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 0 );

	// Start dispatching
	scheduler.StartDispatching();

	// Worker will find and execute work
	executingWorker->ExecuteUntilStarved();
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 1 );

	// Worker will fail to find work
	executingWorker->ExecuteUntilStarved();
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 1 );

	// Stop dispatching
	scheduler.StopDispatching( true );

	// Worker will fail to find work
	ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
	executingWorker->ExecuteUntilStarved();
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 1 );

	// Unregister pool
	scheduler.UnregisterPool( pool );

	// Worker will fail to find work
	ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
	executingWorker->ExecuteUntilStarved();
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 1 );

	// Unregister worker
	{
		// Unregister
		scheduler.UnregisterWorker( worker.Weaken() );
		ASSERT_NE( strongWorker, nullptr );
		ASSERT_NE( worker.Weaken(), nullptr );
		ASSERT_NE( executingWorker, nullptr );

		// Worker will fail to find work
		ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
		executingWorker->ExecuteUntilStarved();
		ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 1 );

		// Stop executing worker
		strongWorker = nullptr;
		ASSERT_EQ( worker.Weaken(), nullptr );
		ASSERT_EQ( executingWorker, nullptr );
	}
}



TEST( Scheduling, MultiThreadedSchedulerLifetime )
{
	TaskScheduler scheduler;
	details::sU8Val.store( 0, rftl::memory_order::memory_order_release );

	// Register worker
	WeakSharedPtr<TaskWorker> worker;
	WeakPtr<ThreadableTaskWorker> executingWorker;
	{
		UniquePtr<ThreadableTaskWorker> newWorker = DefaultCreator<ThreadableTaskWorker>::Create();
		executingWorker = newWorker;

		worker = scheduler.RegisterWorker( rftl::move( newWorker ) );
		ASSERT_EQ( newWorker, nullptr );
		ASSERT_NE( worker.Weaken(), nullptr );
		ASSERT_EQ( worker.Weaken(), executingWorker );
	}

	// Keep worker alive while potentially executing
	SharedPtr<TaskWorker> strongWorker = worker.Lock();

	// Run the worker asyncronously
	rftl::atomic_bool terminateWorkerThread = false;
	auto threadedWorker = [](
		ThreadableTaskWorker* worker,
		rftl::atomic_bool const* shouldTerminate )->
		void
	{
		while( shouldTerminate->load( rftl::memory_order::memory_order_acquire ) == false )
		{
			worker->ExecuteUntilStarved();
			rftl::this_thread::yield();
		}
	};
	rftl::future<void> threadResultHandle = rftl::async( rftl::launch::async, threadedWorker, static_cast<ThreadableTaskWorker*>( executingWorker ), &terminateWorkerThread );

	// Worker will fail to find work
	ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 0 );

	// Register pool
	WeakPtr<TaskPool> pool;
	WeakPtr<FIFOTaskPool> derivedPool;
	{
		UniquePtr<FIFOTaskPool> newPool = EntwinedCreator<FIFOTaskPool>::Create();
		derivedPool = newPool;

		pool = scheduler.RegisterPool( rftl::move( newPool ), TaskPriority::Normal );
		ASSERT_EQ( newPool, nullptr );
		ASSERT_NE( pool, nullptr );
		ASSERT_EQ( pool, derivedPool );
	}

	// Worker will fail to find work
	ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 0 );

	// Add task
	WeakPtr<Task> task;
	TaskID taskID;
	{
		auto myFunctor = CreateFunctorTask( details::IncU8Val );
		using TaskType = decltype( myFunctor );
		UniquePtr<TaskType> newTask = EntwinedCreator<TaskType>::Create( rftl::move( myFunctor ) );
		task = newTask;

		taskID = pool->AddTask( rftl::move( newTask ) );
		ASSERT_EQ( newTask, nullptr );
		ASSERT_NE( task, nullptr );
		ASSERT_NE( taskID, kInvalidTaskID );
	}

	// Worker will fail to find work
	ASSERT_FALSE( scheduler.AllTasksAreCurrentlyCompleted() );
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 0 );

	// Start dispatching
	scheduler.StartDispatching();

	// Worker will find and execute work
	while( details::sU8Val.load( rftl::memory_order::memory_order_acquire ) == 0 )
	{
		rftl::this_thread::yield();
	}
	ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 1 );

	// Stop dispatching
	scheduler.StopDispatching( true );
	ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );

	// Unregister pool
	scheduler.UnregisterPool( pool );

	// Unregister worker
	{
		// Unregister
		scheduler.UnregisterWorker( worker.Weaken() );
		ASSERT_NE( strongWorker, nullptr );
		ASSERT_NE( worker.Weaken(), nullptr );
		ASSERT_NE( executingWorker, nullptr );

		// Worker will fail to find work
		ASSERT_TRUE( scheduler.AllTasksAreCurrentlyCompleted() );
		executingWorker->ExecuteUntilStarved();
		ASSERT_EQ( details::sU8Val.load( rftl::memory_order::memory_order_acquire ), 1 );
	}

	// Stop executing worker
	terminateWorkerThread.store( true, rftl::memory_order::memory_order_release );
	threadResultHandle.wait();
	strongWorker = nullptr;
	ASSERT_EQ( worker.Weaken(), nullptr );
	ASSERT_EQ( executingWorker, nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}}
