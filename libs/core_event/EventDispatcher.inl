#pragma once
#include "EventDispatcher.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace event {
///////////////////////////////////////////////////////////////////////////////

template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::KeyedHandler::KeyedHandler( HandlerID id, Handler&& handler )
	: mID( rftl::move( id ) )
	, mHandler( rftl::move( handler ) )
{
	//
}



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::KeyedHandler::KeyedHandler( KeyedHandler&& rhs )
	: RF_MOVE_CONSTRUCT( mID )
	, RF_MOVE_CONSTRUCT( mHandler )
{
	//
}



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline typename EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::KeyedHandler& EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::KeyedHandler::operator=( KeyedHandler&& rhs )
{
	if( this != &rhs )
	{
		RF_MOVE_ASSIGN( mID );
		RF_MOVE_ASSIGN( mHandler );
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::HandlerRef::~HandlerRef()
{
	HandlerStorage* const storage = mHandlerStorage.Get();
	if( storage != nullptr )
	{
		HandlerList& handlerList = storage->mHandlerList;
		typename HandlerList::const_iterator toErase = handlerList.end();
		for( typename HandlerList::const_iterator iter = handlerList.begin(); iter != handlerList.end(); iter++ )
		{
			KeyedHandler const& handler = *iter;
			if( handler.mID == mID )
			{
				toErase = iter;
				break;
			}
		}
		RF_ASSERT_MSG( toErase != handlerList.end(), "Handler not found during unregistration" );
		PoliciesT::HandlerOrderPolicy::Remove( handlerList, toErase );
		mHandlerStorage = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::EventDispatcher( EventQueue&& eventQueue )
	: mEventQueue( rftl::move( eventQueue ) )
	, mHandlerStorage( DefaultCreator<HandlerStorage>::Create() )
{
	//
}



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline typename EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::HandlerRef EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::RegisterHandler( Handler&& handler )
{
	mPreviousHandlerID++;
	HandlerID const handlerID = mPreviousHandlerID;

	Policies::HandlerOrderPolicy::Add(
		mHandlerStorage->mHandlerList,
		rftl::move( KeyedHandler{
			handlerID,
			rftl::move( handler ) } ) );

	return HandlerRef{ handlerID, mHandlerStorage };
}



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline bool EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::AddEvent( Event&& event )
{
	return mEventQueue.AddEvent( rftl::move( event ) );
}



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline size_t EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::DispatchEvents()
{
	// Collect any new events
	mEventQueue.StageNewEvents();
	EventList events = mEventQueue.ExtractStagedEvents();

	// Add in any deferred events
	if( Policies::EventDeferPolicy::kCanDefer )
	{
		events.reserve( events.size() + mDeferredEvents.size() );
		for( Event& event : mDeferredEvents )
		{
			events.emplace_back( rftl::move( event ) );
		}
		mDeferredEvents.clear();
	}

	// Early out if there are no events
	if( events.empty() )
	{
		return 0;
	}

	// Sort the events
	if( Policies::EventSortPolicy::kCanSort )
	{
		Policies::EventSortPolicy::Sort( events.begin(), events.end() );
	}

	HandlerList& handlers = mHandlerStorage->mHandlerList;

	// For each event...
	size_t const eventsConsidered = events.size();
	for( Event& rawEvent : events )
	{
		// Respect the mutability policy while processing
		// NOTE: This may make it 'Event const&'
		using DispatchableEvent = typename Policies::EventMutablePolicy::template DispatchableEvent<Event>;
		DispatchableEvent& dispatchableEvent = rawEvent;
		Event& modifiableEvent = rawEvent;

		// Consider deferring
		if( Policies::EventDeferPolicy::kCanDefer )
		{
			if( Policies::EventDeferPolicy::ShouldDefer( dispatchableEvent ) )
			{
				mDeferredEvents.emplace_back( rftl::move( modifiableEvent ) );
				continue;
			}
		}

		// Consider discarding
		if( Policies::EventDiscardPolicy::kCanDiscard )
		{
			if( Policies::EventDiscardPolicy::ShouldDiscard( dispatchableEvent ) )
			{
				continue;
			}
		}

		// For each handler...
		for( KeyedHandler& keyedHandler : handlers )
		{
			Handler& handler = keyedHandler.mHandler;

			// Consider deferring
			if( Policies::HandlerPredeterminismPolicy::kCheckDeferBeforeEachHandler )
			{
				if( Policies::EventDeferPolicy::kCanDefer )
				{
					if( Policies::EventDeferPolicy::ShouldDefer( dispatchableEvent ) )
					{
						// WARNING: This is really wierd, since some handlers may
						//  have already processed it, and may process it again as
						//  a result of it being deferred
						mDeferredEvents.emplace_back( rftl::move( modifiableEvent ) );
						break;
					}
				}
			}

			// Consider discarding
			if( Policies::HandlerPredeterminismPolicy::kCheckDiscardBeforeEachHandler )
			{
				if( Policies::EventDiscardPolicy::kCanDiscard )
				{
					if( Policies::EventDiscardPolicy::ShouldDiscard( dispatchableEvent ) )
					{
						break;
					}
				}
			}

			// Consider skipping
			if( Policies::HandlerSkipPolicy::kCanSkip )
			{
				if( Policies::HandlerSkipPolicy::ShouldSkip( handler, dispatchableEvent ) )
				{
					continue;
				}
			}

			// Handle
			Policies::HandlerDispatchPolicy::Dispatch( handler, dispatchableEvent );
		}
	}

	size_t const eventsDeferred = mDeferredEvents.size();
	RF_ASSERT( eventsDeferred <= eventsConsidered );
	return eventsConsidered - eventsDeferred;
}

///////////////////////////////////////////////////////////////////////////////
}}
