#pragma once
#include "SinglethreadEventQueue.h"


namespace RF { namespace event {
///////////////////////////////////////////////////////////////////////////////

template<typename EventT>
inline SinglethreadEventQueue<EventT>::SinglethreadEventQueue()
{
	//
}



template<typename EventT>
inline SinglethreadEventQueue<EventT>::SinglethreadEventQueue( SinglethreadEventQueue&& rhs )
	: mQueue( rftl::move( rhs.mQueue ) )
	, mNumStaged( rftl::move( rhs.mNumStaged ) )
{
	//
}



template<typename EventT>
inline SinglethreadEventQueue<EventT>& SinglethreadEventQueue<EventT>::operator=( SinglethreadEventQueue&& rhs )
{
	if( this != &rhs )
	{
		mQueue = rftl::move( rhs.mQueue );
		mNumStaged = rftl::move( rhs.mNumStaged );
	}
	return *this;
}



template<typename EventT>
inline bool SinglethreadEventQueue<EventT>::AddEvent( Event&& event )
{
	mQueue.emplace_back( rftl::move( event ) );
	return true;
}



template<typename EventT>
inline size_t SinglethreadEventQueue<EventT>::StageNewEvents()
{
	size_t const newlyStaged = mQueue.size() - mNumStaged;
	mNumStaged = mQueue.size();
	return newlyStaged;
}



template<typename EventT>
inline typename SinglethreadEventQueue<EventT>::EventQueue SinglethreadEventQueue<EventT>::ExtractStagedEvents()
{
	if( mQueue.size() == mNumStaged )
	{
		EventQueue const retVal = rftl::move( mQueue );
		mNumStaged = 0;
		return retVal;
	}

	// Don't do this! Always stage first! Not bothering to make performant...
	EventQueue retVal;
	while( mNumStaged > 0 )
	{
		// Move front
		retVal.emplace_back( rftl::move( mQueue.front() ) );

		// Pop front
		mQueue.erase( mQueue.begin() );
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
