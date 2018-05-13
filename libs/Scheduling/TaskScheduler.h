#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core/ptr/weak_shared_ptr.h"
#include "core/ptr/unique_ptr.h"

#include "rftl/vector"
#include "rftl/atomic"
#include "rftl/shared_mutex"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API TaskScheduler
{
	RF_NO_COPY( TaskScheduler );

	//
	// Friends and forwards
protected:
	friend class TaskWorker;
	friend class TaskPool;
private:
	struct RegisteredWorker;
	struct RegisteredPool;
	struct PriorityBlock;


	//
	// Types and constants
private:
	using WorkerList = rftl::vector<RegisteredWorker>;
	static constexpr size_t kNumPriorities = 4; // SEE: TaskPriority enum
	using PoolList = rftl::vector<RegisteredPool>;
	using PriorityBlockList = PriorityBlock[kNumPriorities];
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Structs
private:
	struct RegisteredWorker
	{
		RF_NO_COPY( RegisteredWorker );

		explicit RegisteredWorker( SharedPtr<TaskWorker> const& worker );
		RegisteredWorker( RegisteredWorker&& rhs );
		RegisteredWorker& operator=( RegisteredWorker && rhs );

		SharedPtr<TaskWorker> mWorker = nullptr;
		TaskID mActiveTaskID = kInvalidTaskID;
		Task* mActiveTask = nullptr;
		WeakPtr<TaskPool> mActivePool = nullptr;

		rftl::atomic_bool mFlaggedAsActive = false;
		rftl::atomic_bool mFlaggedForUnregistration = false;
	};

	struct RegisteredPool
	{
		RF_NO_COPY( RegisteredPool );

		explicit RegisteredPool( UniquePtr<TaskPool> && pool );
		RegisteredPool( RegisteredPool&& rhs );
		RegisteredPool& operator=( RegisteredPool && rhs );

		UniquePtr<TaskPool> mPool = nullptr;
	};

	struct PriorityBlock
	{
		RF_NO_COPY( PriorityBlock );

		PriorityBlock() = default;

		TaskPriority mPriority = TaskPriority::Invalid;
		PoolList mPools;
		rftl::atomic<size_t> mLastDispatchIndex = 0;
	};



	//
	// Public methods
public:
	// NOTE: Scheduler begins with dispatching disabled, and waits for it to
	//  become enabled by caller
	TaskScheduler();

	// Register/unregister workers
	// NOTE: Effects not immediate, tasks may continue to be dispatched and
	//  processed by workers as though these calls were not made
	WeakSharedPtr<TaskWorker> RegisterWorker( UniquePtr<TaskWorker> && worker );
	void UnregisterWorker( WeakPtr<TaskWorker> const& worker );

	// Add/remove task pools
	// NOTE: Pools with higher priority will have their tasks dispatched first,
	//  at the expense of lower priority pools, possibly to an extreme of
	//  preventing the lower priority from running until all higher priority
	//  pools have completed all their tasks or yielded
	// NOTE: Pools with the same priority will have more balanced dispatching
	WeakPtr<TaskPool> RegisterPool( UniquePtr<TaskPool> && pool, TaskPriority priority );
	void UnregisterPool( WeakPtr<TaskPool> const& pool );

	// Start/stop dispatching
	// NOTE: Effects not immediate, tasks may continue to be dispatched and
	//  processed as though these calls were not made
	// NOTE: Only applies to this scheduler, workers may be shared with other
	//  schedulers in some cases
	void StartDispatching();
	void StopDispatching( bool blockUntilWorkersIdle );

	// Performs an instant check that all tasks are complete, there is no
	//  further work to dispatch, and all workers registered with the scheduler
	//  are not performing tasks on its behalf
	// NOTE: Only applies to this scheduler, workers may be shared with other
	//  schedulers in some cases
	// WARNING: It is unsafe to perform any actions based on the result of this
	//  call unless the caller controls the tasks the scheduler works on and
	//  can prevent new tasks from being introduced
	bool AllTasksAreCurrentlyCompleted() const;


	//
	// Protected methods
protected:
	// Called by workers
	void OnWorkComplete( TaskWorker* worker, Task* task, TaskState newState );

	// Called by pools
	void OnTasksNewlyAvailable( TaskPool* pool );


	//
	// Private methods
private:
	void AttemptDispatch();
	bool AttemptDispatch( RegisteredWorker& worker );

	bool AreAllWorkersIdle() const;

	PriorityBlock& GetPriorityBlock( TaskPriority priority );


	//
	// Private data
private:
	PriorityBlockList mPrioritizedPools;
	mutable ReaderWriterMutex mPoolLock;

	WorkerList mWorkers;
	mutable ReaderWriterMutex mWorkerLock;

	rftl::atomic_bool mDispatchingPermitted = false;
};

///////////////////////////////////////////////////////////////////////////////
}}
