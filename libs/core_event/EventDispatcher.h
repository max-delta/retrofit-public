#pragma once
#include "core/macros.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"

#include "rftl/vector"
#include "rftl/mutex"
#include "rftl/shared_mutex"


namespace RF { namespace event {
///////////////////////////////////////////////////////////////////////////////
namespace traits {

// Should event handlers be allowed to modify events, potentially changing
//  further dispatch logic and other handlers?
struct EventsAreMutable
{
	template<typename EventT>
	using DispatchableEvent = EventT;
};
struct EventsAreImmutable
{
	template<typename EventT>
	using DispatchableEvent = EventT const;
};



// Should handlers be run in a specifc order?
template<typename SortT>
struct HandlersAreRunInSortOrder
{
	template<typename ContainerT, typename HandlerT>
	static void Add( ContainerT& container, HandlerT&& handler )
	{
		container.emplace_back( rftl::move( handler ) );
		SortT( container.begin(), container.end() );
	}
	template<typename ContainerT, typename IterT>
	static void Remove( ContainerT& container, IterT const& toErase )
	{
		rftl::swap( *toErase, container.back() );
		container.pop_back();
		SortT( container.begin(), container.end() );
	}
};
struct HandlersAreRunInRegistrationOrder
{
	template<typename ContainerT, typename HandlerT>
	static void Add( ContainerT& container, HandlerT&& handler )
	{
		container.emplace_back( rftl::move( handler ) );
	}
	template<typename ContainerT, typename IterT>
	static void Remove( ContainerT& container, IterT const& toErase )
	{
		container.erase( toErase );
	}
};
struct HandlersAreRunInArbitraryOrder
{
	template<typename ContainerT, typename HandlerT>
	static void Add( ContainerT& container, HandlerT&& handler )
	{
		container.emplace_back( rftl::move( handler ) );
	}
	template<typename ContainerT, typename IterT>
	static void Remove( ContainerT& container, IterT const& toErase )
	{
		rftl::swap( *toErase, container.back() );
		container.pop_back();
	}
};



// Should some handlers skip certain events?
template<typename SkipT>
struct HandlersAreConditional
{
	static constexpr bool kCanSkip = true;
	template<typename HandlerT, typename EventT>
	static bool ShouldSkip( HandlerT& handler, EventT& event )
	{
		return SkipT( handler, event );
	}
};
struct HandlersAreUnconditional
{
	static constexpr bool kCanSkip = false;
	template<typename HandlerT, typename EventT>
	static bool ShouldSkip( HandlerT& handler, EventT& event )
	{
		return false;
	}
};



// Do handlers require an additional layer of processing?
template<typename DispatchT>
struct HandlerCustomDispatch
{
	template<typename HandlerT, typename EventT>
	static void Dispatch( HandlerT& handler, EventT& event )
	{
		return DispatchT( handler, event );
	}
};
struct HandlerCallDispatch
{
	template<typename HandlerT, typename EventT>
	static void Dispatch( HandlerT& handler, EventT& event )
	{
		return handler( event );
	}
};



// Should events be sorted before dispatch?
template<typename SortT>
struct EventsAreSorted
{
	static constexpr bool kCanSort = true;
	template<typename IterT>
	static void Sort( IterT first, IterT last )
	{
		SortT( first, last );
	}
};
struct EventsAreUnsorted
{
	static constexpr bool kCanSort = false;
	template<typename IterT>
	static void Sort( IterT first, IterT last )
	{
		//
	}
};



// Should some events be discarded?
template<typename DiscardT>
struct EventsAreDiscardable
{
	static constexpr bool kCanDiscard = true;
	template<typename EventT>
	static bool ShouldDiscard( EventT& event )
	{
		return DiscardT( event );
	}
};
struct EventsAreNotDiscardable
{
	static constexpr bool kCanDiscard = false;
	template<typename EventT>
	static bool ShouldDiscard( EventT& event )
	{
		return false;
	}
};



// Should some events be deferred?
template<typename DeferT>
struct EventsAreDeferrable
{
	static constexpr bool kCanDefer = true;
	template<typename EventT>
	static bool ShouldDefer( EventT& event )
	{
		return DeferT( event );
	}
};
struct EventsAreNotDeferrable
{
	static constexpr bool kCanDefer = false;
	template<typename EventT>
	static bool ShouldDefer( EventT& event )
	{
		return false;
	}
};



// Can the act of calling a handler modify the logic for further handlers?
struct HandlingRequiresReevaluation
{
	static constexpr bool kCheckDiscardBeforeEachHandler = true;
	static constexpr bool kCheckDeferBeforeEachHandler = true;
};
struct HandlingIsPredeterministic
{
	static constexpr bool kCheckDiscardBeforeEachHandler = false;
	static constexpr bool kCheckDeferBeforeEachHandler = false;
};
}
///////////////////////////////////////////////////////////////////////////////

struct EventDispatcherDefaultPolicies
{
	using EventMutablePolicy = traits::EventsAreImmutable;
	using HandlerOrderPolicy = traits::HandlersAreRunInRegistrationOrder;
	using HandlerSkipPolicy = traits::HandlersAreUnconditional;
	using HandlerDispatchPolicy = traits::HandlerCallDispatch;
	using EventSortPolicy = traits::EventsAreUnsorted;
	using EventDiscardPolicy = traits::EventsAreNotDiscardable;
	using EventDeferPolicy = traits::EventsAreNotDeferrable;
	using HandlerPredeterminismPolicy = traits::HandlingIsPredeterministic;
};

///////////////////////////////////////////////////////////////////////////////

template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT = EventDispatcherDefaultPolicies>
class EventDispatcher;



template<typename EventT, typename HandlerT, typename EventQueueT, typename PoliciesT>
class EventDispatcher
{
	RF_NO_COPY( EventDispatcher );

	//
	// Forwards
private:
	struct HandlerStorage;
	struct KeyedHandler;
public:
	struct HandlerRef;

	//
	// Types and constants
public:
	using Event = EventT;
	using Handler = HandlerT;
	using EventQueue = EventQueueT;
	using Policies = PoliciesT;

private:
	using HandlerID = uint64_t;
	static constexpr HandlerID kInvalidHandlerID = 0;
	using HandlerList = rftl::vector<KeyedHandler>;
	using HandlerIDList = rftl::vector<HandlerID>;
	using EventList = typename EventQueue::EventQueue;
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;
	using ExclusiveMutex = rftl::mutex;
	using ExclusiveLock = rftl::unique_lock<rftl::mutex>;


	//
	// Structs
private:
	struct HandlerStorage
	{
		RF_NO_COPY( HandlerStorage );

		HandlerStorage() = default;

		HandlerList mHandlerList;
		HandlerList mPendingAdds;
		mutable ReaderWriterMutex mPendingAddMultiReaderSingleWriterLock;
		HandlerIDList mPendingRemoves;
		mutable ReaderWriterMutex mPendingRemoveMultiReaderSingleWriterLock;
	};
	struct KeyedHandler
	{
		RF_NO_COPY( KeyedHandler );

		KeyedHandler( HandlerID id, Handler&& handler );
		KeyedHandler( KeyedHandler&& rhs );
		KeyedHandler& operator=( KeyedHandler&& rhs );

		HandlerID mID = kInvalidHandlerID;
		Handler mHandler;
	};

public:
	struct HandlerRef
	{
		RF_NO_COPY( HandlerRef );

		~HandlerRef();

		HandlerID mID = kInvalidHandlerID;
		WeakPtr<HandlerStorage> mHandlerStorage;
	};



	//
	// Public methods
public:
	EventDispatcher( EventQueue&& eventQueue );

	HandlerRef RegisterHandler( Handler&& handler );

	bool AddEvent( Event&& event );
	size_t DispatchEvents();


	//
	// Private data
private:
	EventQueue mEventQueue;
	EventList mDeferredEvents;
	UniquePtr<HandlerStorage> mHandlerStorage;
	HandlerID mPreviousHandlerID = kInvalidHandlerID;
	ExclusiveMutex mDispatchExclusiveLock;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "EventDispatcher.inl"
