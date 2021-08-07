#pragma once

#include "rftl/unordered_map"


namespace RF::fsm {
///////////////////////////////////////////////////////////////////////////////

// TODO: Templatize hash and storage
// TODO: General cleanup
template<typename TStateID, typename TState>
class GenericStateCollection
{
public:
	using State = TState;
	using StateID = TStateID;
	using Storage = rftl::unordered_map<StateID, State>;


public:
	State const* LookupState( StateID const& identifier ) const
	{
		return &( mStates.at( identifier ) );
	}


	State* LookupMutableState( StateID const& identifier )
	{
		return &( mStates.at( identifier ) );
	}


public:
	Storage mStates;
};

///////////////////////////////////////////////////////////////////////////////
}
