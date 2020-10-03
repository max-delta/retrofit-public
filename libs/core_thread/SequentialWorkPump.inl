#include "stdafx.h"
#include "SequentialWorkPump.h"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

template<typename WorkItemT>
inline SequentialWorkPump<WorkItemT>::~SequentialWorkPump()
{
	if( mWorker.IsStarted() )
	{
		mWorker.Stop();
	}
}



template<typename WorkItemT>
inline void SequentialWorkPump<WorkItemT>::Init(
	AsyncThread::PrepFunc&& prep,
	WorkItemFunc&& exec )
{
	RF_ASSERT( exec != nullptr );
	mWork = rftl::move( exec );
	auto doWork = [this]() -> void //
	{
		return this->DoWork();
	};

	auto hasWork = [this]() -> bool //
	{
		return this->HasWork();
	};

	mWorker.Init(
		rftl::move( prep ),
		rftl::move( doWork ),
		rftl::move( hasWork ),
		nullptr );
}



template<typename WorkItemT>
inline bool SequentialWorkPump<WorkItemT>::IsStarted() const
{
	return mWorker.IsStarted();
}



template<typename WorkItemT>
inline void SequentialWorkPump<WorkItemT>::Start()
{
	mWorker.Start();
}



template<typename WorkItemT>
inline void SequentialWorkPump<WorkItemT>::Stop()
{
	mWorker.Stop();
}



template<typename WorkItemT>
inline void SequentialWorkPump<WorkItemT>::AddWorkItem( WorkItem&& workItem )
{
	{
		WriterLock const lock( mWorkItemsMutex );
		mWorkItems.emplace_back( workItem );
	}
	mWorker.Wake();
}



template<typename WorkItemT>
inline bool SequentialWorkPump<WorkItemT>::HasQueuedWork() const
{
	ReaderLock const lock( mWorkItemsMutex );
	return mWorkItems.empty() == false;
}

///////////////////////////////////////////////////////////////////////////////

template<typename WorkItemT>
inline void SequentialWorkPump<WorkItemT>::DoWork()
{
	// Pop
	WorkItem fetch = {};
	{
		WriterLock const lock( mWorkItemsMutex );
		fetch = rftl::move( mWorkItems.front() );
		mWorkItems.pop_front();
	}

	// Execute
	mWork( rftl::move( fetch ) );
}



template<typename WorkItemT>
inline bool SequentialWorkPump<WorkItemT>::HasWork()
{
	ReaderLock const lock( mWorkItemsMutex );
	return mWorkItems.empty() == false;
}

///////////////////////////////////////////////////////////////////////////////
}
