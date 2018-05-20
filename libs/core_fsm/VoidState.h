#pragma once

#include "rftl/cstddef"


namespace RF { namespace fsm {
///////////////////////////////////////////////////////////////////////////////

// Default state to show usage patterns, not intended for actual use
class VoidState
{
public:
	void OnEnter( nullptr_t& context )
	{
		//
	}

	void OnExit( nullptr_t& context )
	{
		//
	}
};

///////////////////////////////////////////////////////////////////////////////
}}
