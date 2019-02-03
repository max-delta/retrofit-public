#include "stdafx.h"

#include "core_event/SinglethreadEventQueue.h"
#include "core_event/EventDispatcher.h"

#include "rftl/functional"


namespace RF { namespace event {
///////////////////////////////////////////////////////////////////////////////

TEST( EventDispatcher, Rudimentary )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void (int const&)>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		// Default
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto onEvent = [&seenEvents]( Event const& event )->void
	{
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	size_t const numDispatched = dispatcher.DispatchEvents();
	ASSERT_EQ( numDispatched, 1 );
	ASSERT_EQ( seenEvents.size(), 1 );
	ASSERT_EQ( seenEvents[0], 1 );
}

///////////////////////////////////////////////////////////////////////////////
}}
