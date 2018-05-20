#pragma once

#include "rftl/unordered_map"


namespace RF { namespace fsm {
///////////////////////////////////////////////////////////////////////////////

// TODO: Templatize hash and storage
// TODO: General cleanup
template<typename TStateID, typename TState>
class GenericStateCollection
{
public:
	using State = TState;
	using StateID = TStateID;


public:
	State const* LookupState( StateID const& identifier ) const
	{
		return &(mStates.at( identifier ));
	}


	State* LookupMutableState( StateID const& identifier )
	{
		return &( mStates.at( identifier ) );
	}


private:
	rftl::unordered_map<StateID, State> mStates;
};

///////////////////////////////////////////////////////////////////////////////
}}
