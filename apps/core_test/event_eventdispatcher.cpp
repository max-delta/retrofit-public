#include "stdafx.h"

#include "core_event/SinglethreadEventQueue.h"
#include "core_event/EventDispatcher.h"

#include "rftl/functional"
#include "rftl/algorithm"


RF_CLANG_IGNORE( "-Wshadow-uncaptured-local" );

namespace RF::event {
///////////////////////////////////////////////////////////////////////////////

TEST( EventDispatcher, Empty )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		// Default
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	Dispatcher dispatcher{ Queue() };
}



TEST( EventDispatcher, EmptyDispatch )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		// Default
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	Dispatcher dispatcher{ Queue() };
	size_t const numDispatched = dispatcher.DispatchEvents();
	ASSERT_EQ( numDispatched, 0 );
}



TEST( EventDispatcher, NoHandlerDispatch )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		// Default
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	Dispatcher dispatcher{ Queue() };

	dispatcher.AddEvent( 1 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
	}
}



TEST( EventDispatcher, NoEventDispatch )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		// Default
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event const& event ) -> void {
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	size_t const numDispatched = dispatcher.DispatchEvents();
	ASSERT_EQ( numDispatched, 0 );
}



TEST( EventDispatcher, EventBeforeHandler )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		// Default
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event const& event ) -> void {
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 1 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 1 );
	}
}



TEST( EventDispatcher, HandlerBeforeEvent )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		// Default
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event const& event ) -> void {
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	ASSERT_EQ( seenEvents.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 1 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 1 );
	}
}



TEST( EventDispatcher, MutableEvent )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		using EventMutablePolicy = traits::EventsAreMutable;
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event& event ) -> void {
		seenEvents.emplace_back( event );
		event++;
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler1 = dispatcher.RegisterHandler( onEvent );
	Dispatcher::HandlerRef const handler2 = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	ASSERT_EQ( seenEvents.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( seenEvents.size(), 2 );
		ASSERT_EQ( seenEvents[0], 1 );
		ASSERT_EQ( seenEvents[1], 2 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( seenEvents.size(), 2 );
		ASSERT_EQ( seenEvents[0], 1 );
		ASSERT_EQ( seenEvents[1], 2 );
	}
}



TEST( EventDispatcher, SortedHandlers )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	struct Handler
	{
		void operator()( Event const& event )
		{
			mVec->emplace_back( mVal );
		}
		int mVal = 0;
		rftl::vector<int>* mVec = nullptr;
	};
	struct Policies : EventDispatcherDefaultPolicies
	{
		struct Sort
		{
			using Iter = rftl::vector<KeyedHandler<Handler>>::iterator;
			void operator()( Iter const& lhs, Iter const& rhs )
			{
				rftl::sort( lhs, rhs, []( KeyedHandler<Handler> const& lhs, KeyedHandler<Handler> const& rhs ) {
					// Reverse order
					return lhs.mHandler.mVal > rhs.mHandler.mVal;
				} );
			}
		};
		using HandlerOrderPolicy = traits::HandlersAreRunInSortOrder<Sort>;
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<int> runHandlers;

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( runHandlers.size(), 0 );

	Dispatcher::HandlerRef const handler1 = dispatcher.RegisterHandler( { 1, &runHandlers } );
	Dispatcher::HandlerRef const handler2 = dispatcher.RegisterHandler( { 2, &runHandlers } );
	ASSERT_EQ( runHandlers.size(), 0 );

	dispatcher.AddEvent( 1 );
	ASSERT_EQ( runHandlers.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( runHandlers.size(), 2 );
		ASSERT_EQ( runHandlers[0], 2 );
		ASSERT_EQ( runHandlers[1], 1 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( runHandlers.size(), 2 );
		ASSERT_EQ( runHandlers[0], 2 );
		ASSERT_EQ( runHandlers[1], 1 );
	}
}



TEST( EventDispatcher, SkippedHandlers )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	struct Handler
	{
		void operator()( Event const& event )
		{
			mVec->emplace_back( mVal );
		}
		int mVal = 0;
		rftl::vector<int>* mVec = nullptr;
	};
	struct Policies : EventDispatcherDefaultPolicies
	{
		struct Skip
		{
			bool operator()( Handler const& handler, Event const& event )
			{
				return handler.mVal == 1;
			}
		};
		using HandlerSkipPolicy = traits::HandlersAreConditional<Skip>;
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<int> runHandlers;

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( runHandlers.size(), 0 );

	Dispatcher::HandlerRef const handler1 = dispatcher.RegisterHandler( { 1, &runHandlers } );
	Dispatcher::HandlerRef const handler2 = dispatcher.RegisterHandler( { 2, &runHandlers } );
	ASSERT_EQ( runHandlers.size(), 0 );

	dispatcher.AddEvent( 1 );
	ASSERT_EQ( runHandlers.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( runHandlers.size(), 1 );
		ASSERT_EQ( runHandlers[0], 2 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( runHandlers.size(), 1 );
		ASSERT_EQ( runHandlers[0], 2 );
	}
}



TEST( EventDispatcher, CustomDispatch )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		struct Dispatch
		{
			void operator()( Handler& handler, Event const& event )
			{
				handler( event + 1 );
			}
		};
		using HandlerDispatchPolicy = traits::HandlerCustomDispatch<Dispatch>;
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event const& event ) -> void {
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	ASSERT_EQ( seenEvents.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 2 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 2 );
	}
}



TEST( EventDispatcher, SortedEvents )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		struct Sort
		{
			using Iter = rftl::vector<Event>::iterator;
			void operator()( Iter const& lhs, Iter const& rhs )
			{
				rftl::sort( lhs, rhs, []( Event const& lhs, Event const& rhs ) {
					// Reverse order
					return lhs > rhs;
				} );
			}
		};
		using EventSortPolicy = traits::EventsAreSorted<Sort>;
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event const& event ) -> void {
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	dispatcher.AddEvent( 2 );
	ASSERT_EQ( seenEvents.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 2 );
		ASSERT_EQ( seenEvents.size(), 2 );
		ASSERT_EQ( seenEvents[0], 2 );
		ASSERT_EQ( seenEvents[1], 1 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( seenEvents.size(), 2 );
		ASSERT_EQ( seenEvents[0], 2 );
		ASSERT_EQ( seenEvents[1], 1 );
	}
}



TEST( EventDispatcher, DiscardEvents )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		struct Discard
		{
			bool operator()( Event const& event )
			{
				return event == 1;
			}
		};
		using EventDiscardPolicy = traits::EventsAreDiscardable<Discard>;
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event const& event ) -> void {
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	dispatcher.AddEvent( 2 );
	ASSERT_EQ( seenEvents.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 2 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 2 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 2 );
	}
}



TEST( EventDispatcher, DeferEvents )
{
	using Event = int;
	using Queue = SinglethreadEventQueue<Event>;
	using Handler = rftl::function<void( int const& )>;
	struct Policies : EventDispatcherDefaultPolicies
	{
		struct Defer
		{
			bool operator()( Event const& event )
			{
				static bool hasDeferred = false;
				if( event == 1 && hasDeferred == false )
				{
					hasDeferred = true;
					return true;
				}
				return false;
			}
		};
		using EventDeferPolicy = traits::EventsAreDeferrable<Defer>;
	};
	using Dispatcher = EventDispatcher<Event, Handler, Queue, Policies>;

	rftl::vector<Event> seenEvents;
	auto const onEvent = [&seenEvents]( Event const& event ) -> void {
		seenEvents.emplace_back( event );
	};

	Dispatcher dispatcher{ Queue() };
	ASSERT_EQ( seenEvents.size(), 0 );

	Dispatcher::HandlerRef const handler = dispatcher.RegisterHandler( onEvent );
	ASSERT_EQ( seenEvents.size(), 0 );

	dispatcher.AddEvent( 1 );
	dispatcher.AddEvent( 2 );
	ASSERT_EQ( seenEvents.size(), 0 );

	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( seenEvents.size(), 1 );
		ASSERT_EQ( seenEvents[0], 2 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 1 );
		ASSERT_EQ( seenEvents.size(), 2 );
		ASSERT_EQ( seenEvents[0], 2 );
		ASSERT_EQ( seenEvents[1], 1 );
	}
	{
		size_t const numDispatched = dispatcher.DispatchEvents();
		ASSERT_EQ( numDispatched, 0 );
		ASSERT_EQ( seenEvents.size(), 2 );
		ASSERT_EQ( seenEvents[0], 2 );
		ASSERT_EQ( seenEvents[1], 1 );
	}
}



TEST( EventDispatcher, HandlerReevaluateDiscard )
{
	// TODO: Enable re-evalutation and test discard
}



TEST( EventDispatcher, HandlerReevaluateDefer )
{
	// TODO: Enable re-evalutation and test defer
}

///////////////////////////////////////////////////////////////////////////////
}
