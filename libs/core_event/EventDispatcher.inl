#pragma once
#include "EventDispatcher.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace event {
///////////////////////////////////////////////////////////////////////////////

template<typename HandlerT>
inline KeyedHandler<HandlerT>::KeyedHandler( HandlerID id, Handler&& handler )
	: mID( rftl::move( id ) )
	, mHandler( rftl::move( handler ) )
{
	//
}



template<typename HandlerT>
inline KeyedHandler<HandlerT>::KeyedHandler( KeyedHandler&& rhs )
	: RF_MOVE_CONSTRUCT( mID )
	, RF_MOVE_CONSTRUCT( mHandler )
{
	RF_MOVE_CLEAR( mID );
	RF_MOVE_CLEAR( mHandler );
}



template<typename HandlerT>
inline KeyedHandler<HandlerT>& KeyedHandler<HandlerT>::operator=( KeyedHandler&& rhs )
{
	if( this != &rhs )
	{
		RF_MOVE_ASSIGN_CLEAR( mID );
		RF_MOVE_ASSIGN_CLEAR( mHandler );
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
		WriterLock lock( storage->mPendingRemoveMultiReaderSingleWriterLock );
		storage->mPendingRemoves.emplace_back( mID );
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
	HandlerStorage* const storage = mHandlerStorage.Get();

	HandlerID handlerID = kInvalidHandlerID;
	{
		WriterLock lock( storage->mPendingAddMultiReaderSingleWriterLock );

		mPreviousHandlerID++;
		handlerID = mPreviousHandlerID;

		storage->mPendingAdds.emplace_back(
			rftl::move( KeyedHandler{
				handlerID,
				rftl::move( handler ) } ) );
	}

	return HandlerRef( handlerID, mHandlerStorage );
}



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline bool EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::AddEvent( Event&& event )
{
	return mEventQueue.AddEvent( rftl::move( event ) );
}



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
inline size_t EventDispatcher<EventT, HandlerT, EventQueueT, PoliciesT>::DispatchEvents()
{
	// Only one dispatch can run at a time, and it's a bad idea anyways to try
	//  and have multiple threads sharing event-processing work, since at that
	//  point a task system is probably a better choice
	ExclusiveLock const dispatchLock( mDispatchExclusiveLock );

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

	// Update handlers
	HandlerStorage* const storage = mHandlerStorage.Get();
	HandlerList& handlers = storage->mHandlerList;
	{
		// Get adds and removes
		HandlerList pendingAdds;
		HandlerIDList pendingRemoves;
		{
			// NOTE: Keeping the 'Add' lock while reading 'Remove's, to prevent
			//  getting a remove without the corresponding add
			WriterLock addLock( storage->mPendingAddMultiReaderSingleWriterLock );
			WriterLock removeLock( storage->mPendingRemoveMultiReaderSingleWriterLock );
			pendingAdds = rftl::move( storage->mPendingAdds );
			pendingRemoves = rftl::move( storage->mPendingRemoves );
		}

		// Process adds
		handlers.reserve( handlers.size() + pendingAdds.size() );
		for( KeyedHandler& handler : pendingAdds )
		{
			PoliciesT::HandlerOrderPolicy::Add( handlers, rftl::move( handler ) );
		}

		// Process removes
		for( HandlerID const& handlerID : pendingRemoves )
		{
			typename HandlerList::iterator toErase = handlers.end();
			for( typename HandlerList::iterator iter = handlers.begin(); iter != handlers.end(); iter++ )
			{
				KeyedHandler const& handler = *iter;
				if( handler.mID == handlerID )
				{
					toErase = iter;
					break;
				}
			}
			RF_ASSERT_MSG( toErase != handlers.end(), "Handler not found during unregistration" );
			PoliciesT::HandlerOrderPolicy::Remove( handlers, toErase );
		}
	}

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
