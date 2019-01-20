#pragma once
#include "core_math/Hash.h"
#include "core/meta/Empty.h"
#include "core/macros.h"

#include "rftl/unordered_map"
#include "rftl/unordered_set"


namespace RF { namespace logic {
///////////////////////////////////////////////////////////////////////////////

template<typename TStateID, typename TStateValue>
struct State
{
	using StateID = TStateID;
	using StateValue = TStateValue;

	State() = default;

	State( StateID const& stateID, StateValue const& stateValue )
		: mStateID( stateID )
		, mStateValue( stateValue )
	{
		//
	}

	State( StateID&& stateID, StateValue&& stateValue )
		: mStateID( rftl::move( stateID ) )
		, mStateValue( rftl::move( stateValue ) )
	{
		//
	}

	bool operator==( State const& rhs ) const
	{
		return mStateID == rhs.mStateID && mStateValue == rhs.mStateValue;
	}

	StateID mStateID;
	StateValue mStateValue;
};



template<
	typename TStateID,
	typename TStateValue,
	typename THash = rftl::hash<TStateID>,
	typename TEquals = rftl::equal_to<TStateID>,
	typename TAlloc = rftl::allocator<TStateID>>
struct StateCollection
{
public:
	struct StateHash;
	struct StateEquals;

public:
	using Hash = THash;
	using Equals = TEquals;
	using Allocator = TAlloc;
	using AllocatorTraits = rftl::allocator_traits<Allocator>;
	using State = State<TStateID, TStateValue>;
private:
	using ReboundEntryAllocator = typename AllocatorTraits::template rebind_alloc<State>;
public:
	using States = rftl::unordered_set<State, StateHash, StateEquals, ReboundEntryAllocator>;

public:
	struct StateHash
	{
		size_t operator()( State const& value ) const
		{
			return Hash()( value.mStateID );
		}
	};
	struct StateEquals
	{
		bool operator()( State const& lhs, State const& rhs ) const
		{
			return Equals()( lhs.mStateID, rhs.mStateID );
		}
	};

public:
	States mStates;
};



template<
	typename TStateID,
	typename TStateValue,
	typename TMetaValue = EmptyStruct,
	typename THash = rftl::hash<TStateID>,
	typename TEquals = rftl::equal_to<TStateID>,
	typename TAlloc = rftl::allocator<TStateID>>
struct Action
{
	using Preconditions = StateCollection<TStateID, TStateValue, THash, TEquals, TAlloc>;
	using Postconditions = StateCollection<TStateID, TStateValue, THash, TEquals, TAlloc>;
	using MetaValue = TMetaValue;
	Preconditions mPreconditions;
	Postconditions mPostconditions;
	MetaValue mMeta;
};



template<
	typename TStateID,
	typename TStateValue,
	typename TMetaValue = EmptyStruct,
	typename THash = rftl::hash<TStateID>,
	typename TEquals = rftl::equal_to<TStateID>,
	typename TAlloc = rftl::allocator<TStateID>>
class ActionDatabase
{
	//
	// Types and constants
public:
	using Hash = THash;
	using Equals = TEquals;
	using Allocator = TAlloc;
	using AllocatorTraits = rftl::allocator_traits<Allocator>;
	using Action = Action<TStateID, TStateValue, TMetaValue, Hash, Equals, Allocator>;
	using ActionID = uint64_t;
	static constexpr ActionID kInvalidActionID = 0;
private:
	using ReboundPair = rftl::pair<ActionID const, Action>;
	using ReboundPairAllocator = typename AllocatorTraits::template rebind_alloc<ReboundPair>;
	using ReboundEntryAllocator = typename AllocatorTraits::template rebind_alloc<ActionID>;
	using ActionsByID = rftl::unordered_map<ActionID, Action, rftl::hash<ActionID>, rftl::equal_to<ActionID>, ReboundPairAllocator>;
public:
	using ActionIDCollection = rftl::unordered_set<ActionID, rftl::hash<ActionID>, rftl::equal_to<ActionID>, ReboundEntryAllocator>;


	//
	// Public methods
public:
	ActionDatabase() = default;

	ActionID AddAction( Action action )
	{
		ActionID const newActionID = ++mActionIDGenerator;
		mActionsByID[newActionID] = action;
		return newActionID;
	}

	Action const* LookupAction( ActionID actionID ) const
	{
		typename ActionsByID::const_iterator const iter = mActionsByID.find( actionID );
		if( iter == mActionsByID.end() )
		{
			return nullptr;
		}
		return &( iter->second );
	}

	ActionIDCollection FindActionsWithPostCondition( typename Action::Postconditions::State const& desiredCondition )
	{
		ActionIDCollection retVal;
		for( typename ActionsByID::value_type const& actionPair : mActionsByID )
		{
			ActionID const actionID = actionPair.first;
			Action const& action = actionPair.second;
			typename Action::Postconditions const& postConditions = action.mPostconditions;
			for( typename Action::Postconditions::State const& condition : postConditions.mStates )
			{
				if( condition == desiredCondition )
				{
					retVal.emplace( actionID );
					break;
				}
			}
		}

		return retVal;
	}


	//
	// Private data
private:
	ActionID mActionIDGenerator = kInvalidActionID;
	ActionsByID mActionsByID;
};

///////////////////////////////////////////////////////////////////////////////
}}
