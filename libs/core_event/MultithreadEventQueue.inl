#pragma once
#include "MultithreadEventQueue.h"


namespace RF { namespace event {
///////////////////////////////////////////////////////////////////////////////

template<typename EventT>
inline MultithreadEventQueue<EventT>::MultithreadEventQueue()
{
	//
}



template<typename EventT>
inline bool MultithreadEventQueue<EventT>::AddEvent( Event&& event )
{
	ThreadID const currentThreadID = rftl::this_thread::get_id();

	// Do we have storage for this thread yet?
	ReaderLock readLock( mEventingThreadQueues.mMultiReaderSingleWriterLock );
	if( mEventingThreadQueues.mThreadQueues.count(currentThreadID) == 0 )
	{
		// No, we will need to create it
		readLock.unlock();
		{
			WriterLock const writeLock( mEventingThreadQueues.mMultiReaderSingleWriterLock );
			mEventingThreadQueues.mThreadQueues[currentThreadID];
		}
		readLock.lock();
	}
	ThreadEventQueue& threadEventQueue = mEventingThreadQueues.mThreadQueues.at( currentThreadID );

	// Store the event
	{
		WriterLock const queueLock( threadEventQueue.mMultiReaderSingleWriterLock );
		threadEventQueue.mEventQueue.emplace_back( rftl::move( event ) );
	}

	return true;
}



template<typename EventT>
inline size_t MultithreadEventQueue<EventT>::StageNewEvents()
{
	// Lock out new threads
	ReaderLock const threadsLock( mEventingThreadQueues.mMultiReaderSingleWriterLock );
	ThreadQueues& threadQueues = mEventingThreadQueues.mThreadQueues;

	// Lock out anyone else trying to stage or extract (they shouldn't be)
	WriterLock const globalLock( mStagedEventQueue.mMultiReaderSingleWriterLock );
	EventQueue& stagedQueue = mStagedEventQueue.mEventQueue;

	// For each thread...
	size_t numStaged = 0;
	for( typename ThreadQueues::value_type& threadPair : threadQueues )
	{
		ThreadEventQueue& thread = threadPair.second;

		// Lock out existing thread
		ReaderLock const threadLock( thread.mMultiReaderSingleWriterLock );
		EventQueue& threadQueue = thread.mEventQueue;

		// For each event...
		stagedQueue.reserve( stagedQueue.size() + threadQueue.size() );
		for( Event& event : threadQueue )
		{
			// Stage it
			stagedQueue.emplace_back( rftl::move( event ) );
			numStaged++;
		}

		// Clear the buffer
		threadQueue.clear();
	}

	return numStaged;
}



template<typename EventT>
inline typename MultithreadEventQueue<EventT>::EventQueue MultithreadEventQueue<EventT>::ExtractStagedEvents()
{
	// Lock out anyone else trying to stage or extract (they shouldn't be)
	WriterLock const globalLock( mStagedEventQueue.mMultiReaderSingleWriterLock );
	EventQueue& stagedQueue = mStagedEventQueue.mEventQueue;

	// Extract the staged events
	EventQueue const retVal = rftl::move( stagedQueue );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
