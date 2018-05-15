#include "stdafx.h"
#include "ThreadableTaskWorker.h"

#include "Scheduling/tasks/Task.h"
#include "Logging/Logging.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

ThreadableTaskWorker::~ThreadableTaskWorker()
{
	bool const lockSuccess = mExecutionMutex.try_lock();
	if( lockSuccess )
	{
		mExecutionMutex.unlock();
	}
	else
	{
		RFLOG_FATAL( nullptr, RFCAT_SCHEDULING,
			"Threadable worker being destroyed while holding an execution lock."
			" Not only is this behavior undefined by the standard, it's also an"
			" indication that another thread is running code that this"
			" destruction is about to invalidate" );
	}
}



void ThreadableTaskWorker::AddTask( Task * task, TaskScheduler * scheduler )
{
	rftl::unique_lock<rftl::mutex> workLock( mPendingWorkMutex );
	mPendingWork.emplace_back( WorkItem{ task, scheduler } );
}



size_t ThreadableTaskWorker::ExecuteUntilStarved()
{
	rftl::unique_lock<rftl::mutex> execLock( mExecutionMutex );

	size_t numExecutions = 0;
	while( true )
	{
		// Pull work
		WorkItem workItem;
		{
			rftl::unique_lock<rftl::mutex> workLock( mPendingWorkMutex );
			if( mPendingWork.empty() )
			{
				return numExecutions;
			}
			workItem = mPendingWork.back();
			mPendingWork.pop_back();
		}

		// Execute
		{
			TaskState const newTaskState = workItem.mTask->Step();
			OnWorkComplete( workItem.mTask, newTaskState, workItem.mScheduler );
			numExecutions++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
