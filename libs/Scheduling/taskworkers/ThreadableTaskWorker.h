#pragma once
#include "project.h"

#include "Scheduling/taskworkers/TaskWorker.h"

#include "rftl/vector"
#include "rftl/mutex"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API ThreadableTaskWorker : public TaskWorker
{
	RF_NO_COPY( ThreadableTaskWorker );

	//
	// Structs
private:
	struct WorkItem
	{
		Task* mTask;
		TaskScheduler* mScheduler;
	};


	//
	// Public methods
public:
	ThreadableTaskWorker() = default;
	virtual ~ThreadableTaskWorker();

	virtual void AddTask( Task* task, TaskScheduler* scheduler ) override;

	size_t ExecuteUntilStarved();


	//
	// Private data
private:
	rftl::mutex mPendingWorkMutex;
	rftl::vector<WorkItem> mPendingWork;

	rftl::mutex mExecutionMutex;
};

///////////////////////////////////////////////////////////////////////////////
}}
