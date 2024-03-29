#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"
#include "core/ptr/shared_ptr.h"

#include "rftl/future"
#include "rftl/unordered_map"
#include "rftl/vector"

// Forwards
namespace RF::scheduling {
class ThreadableTaskWorker;
}


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

class APPCOMMONGRAPHICALCLIENT_API StandardTaskScheduler
{
	RF_NO_COPY( StandardTaskScheduler );

	//
	// Types
private:
	using GenericPoolInterfaceMap = rftl::unordered_map<scheduling::TaskPriority, WeakPtr<scheduling::TaskPool>>;


	//
	// Structs
private:
	struct ThreadedWorker
	{
		RF_NO_COPY( ThreadedWorker );

		ThreadedWorker() = default;

		WeakPtr<scheduling::ThreadableTaskWorker> mWorkerInterface;
		SharedPtr<scheduling::TaskWorker> mWorkerHandle;
		rftl::future<void> mThreadHandle;
		rftl::atomic_bool mTerminateWorker = false;
	};


	//
	// Public methods
public:
	StandardTaskScheduler( bool createMainThreadWorker, size_t workerThreadCount );
	~StandardTaskScheduler();

	WeakPtr<scheduling::TaskScheduler> GetTaskScheduler() const;

	// If the main thread worker is created, then it will need to be run
	//  atleast periodically so that tasks don't clog up on it
	bool HasMainThreadWorkerRequirements() const;
	bool TryExecuteOnMainThreadOnce();
	size_t ExecuteOnMainThreadUntilStarved();


	//
	// Private data
private:
	UniquePtr<scheduling::TaskScheduler> mTaskScheduler;
	rftl::vector<UniquePtr<ThreadedWorker>> mThreadedWorkers;
	UniquePtr<ThreadedWorker> mMainThreadWorker;
	GenericPoolInterfaceMap mGenericPools;
};

///////////////////////////////////////////////////////////////////////////////
}
