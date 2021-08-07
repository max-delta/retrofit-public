#pragma once
#include "core_thread/AsyncThread.h"

#include "rftl/shared_mutex"
#include "rftl/deque"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

// Basic task pump, meant to offload inconsistent work onto another thread in
//  a simple queue-based fashion
template<typename WorkItemT>
class SequentialWorkPump final
{
	RF_NO_COPY( SequentialWorkPump );

	//
	// Types and constants
public:
	using WorkItem = WorkItemT;
	using WorkItemSig = void( WorkItem&& );
	using WorkItemFunc = rftl::function<WorkItemSig>;

private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Public methods
public:
	SequentialWorkPump() = default;
	~SequentialWorkPump();

	// NOTE: Prep function is optional, but intended to be used for setting
	//  thread priorities and thread names
	void Init(
		AsyncThread::PrepFunc&& prep,
		WorkItemFunc&& work );

	// NOTE: Stop is not terminal, work can be started again without re-init
	bool IsStarted() const;
	void Start();
	void Stop();

	void AddWorkItem( WorkItem&& workItem );

	// NOTE: Will return false if the last work item in the queue has left the
	//  queue, but is still being worked on. A call to 'Stop()' in such a case
	//  will wait until the last work item is done being worked on.
	bool HasQueuedWork() const;


	//
	// Private methods
private:
	void DoWork();
	bool HasWork();


	//
	// Private data
private:
	AsyncThread mWorker;

	WorkItemFunc mWork;

	mutable ReaderWriterMutex mWorkItemsMutex;
	rftl::deque<WorkItem> mWorkItems;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "SequentialWorkPump.inl"
