#pragma once
#include "core/macros.h"

#include "rftl/vector"
#include "rftl/unordered_map"


namespace RF::event {
///////////////////////////////////////////////////////////////////////////////

template<typename EventT>
class SinglethreadEventQueue
{
	RF_NO_COPY( SinglethreadEventQueue );

	//
	// Types and constants
public:
	using Event = EventT;
	using EventQueue = rftl::vector<Event>;


	//
	// Public methods
public:
	SinglethreadEventQueue();
	SinglethreadEventQueue( SinglethreadEventQueue&& rhs );
	SinglethreadEventQueue& operator = ( SinglethreadEventQueue && rhs );

	bool AddEvent( Event&& event );
	size_t StageNewEvents();
	EventQueue ExtractStagedEvents();


	//
	// Private data
private:
	EventQueue mQueue;
	size_t mNumStaged = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "SinglethreadEventQueue.inl"
