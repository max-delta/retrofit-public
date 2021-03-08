#pragma once
#include "core/macros.h"

#include "rftl/thread"
#include "rftl/vector"
#include "rftl/unordered_map"
#include "rftl/shared_mutex"


namespace RF::event {
///////////////////////////////////////////////////////////////////////////////

template<typename EventT>
class MultithreadEventQueue
{
	RF_NO_COPY( MultithreadEventQueue );

	//
	// Forwards
private:
	struct ThreadEventQueue;


	//
	// Types and constants
public:
	using Event = EventT;
	using EventQueue = rftl::vector<Event>;
private:
	using ThreadID = rftl::thread::id;
	using ThreadQueues = rftl::unordered_map<ThreadID, ThreadEventQueue>;
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Structs
private:
	struct ThreadEventQueue
	{
		RF_NO_COPY( ThreadEventQueue );
		ThreadEventQueue() = default;
		EventQueue mEventQueue;
		mutable ReaderWriterMutex mMultiReaderSingleWriterLock;
	};

	struct EventingThreadQueues
	{
		RF_NO_COPY( EventingThreadQueues );
		EventingThreadQueues() = default;
		ThreadQueues mThreadQueues;
		mutable ReaderWriterMutex mMultiReaderSingleWriterLock;
	};

	struct StagedEventQueue
	{
		RF_NO_COPY( StagedEventQueue );
		StagedEventQueue() = default;
		EventQueue mEventQueue;
		mutable ReaderWriterMutex mMultiReaderSingleWriterLock;
	};


	//
	// Public methods
public:
	MultithreadEventQueue();

	bool AddEvent( Event&& event );
	size_t StageNewEvents();
	EventQueue ExtractStagedEvents();


	//
	// Private data
private:
	// Events are buffered per-thread here
	EventingThreadQueues mEventingThreadQueues;

	// Events are pulled from threads and buffered globally here
	StagedEventQueue mStagedEventQueue;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "MultithreadEventQueue.inl"
