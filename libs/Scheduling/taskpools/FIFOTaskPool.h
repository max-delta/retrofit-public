#pragma once
#include "project.h"

#include "Scheduling/taskpools/TaskPool.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/deque"
#include "rftl/shared_mutex"


namespace RF::scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API FIFOTaskPool : public TaskPool
{
	RF_NO_COPY( FIFOTaskPool );

	//
	// Forwards
private:
	struct TaskRecord;


	//
	// Types and constants
private:
	using TaskRecordList = rftl::deque<TaskRecord>;
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Structs
private:
	struct TaskRecord
	{
		RF_NO_COPY( TaskRecord );

		TaskRecord( TaskID id, TaskPtr&& task );
		TaskRecord& operator=( TaskRecord&& rhs );

		TaskID mID;
		TaskPtr mTask;
		rftl::atomic_bool mBlocked = false;
		rftl::atomic_bool mAbortPending = false;
	};


	//
	// Public methods
public:
	FIFOTaskPool() = default;
	virtual ~FIFOTaskPool() override;

	virtual TaskID AddTask( TaskPtr&& task ) override;
	virtual void AttemptAbortTask( TaskID taskID ) override;
	virtual void AttemptAbortAllTasks() override;

	virtual void FetchAndBlockNextUnblockedTask( Task*& task, TaskID& taskID ) override;

	virtual void ReturnAndUnblockTask( TaskID taskID, TaskState newState ) override;

	virtual void RemoveBlockedTask( TaskID taskID, TaskState newState ) override;


	//
	// Private data
private:
	TaskRecordList mTaskRecords;
	mutable ReaderWriterMutex mTaskRecordLock;

	TaskIDGenerator mIDGen;
};

///////////////////////////////////////////////////////////////////////////////
}
