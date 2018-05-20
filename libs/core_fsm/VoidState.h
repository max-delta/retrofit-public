#pragma once

#include "rftl/cstddef"


namespace RF { namespace fsm {
///////////////////////////////////////////////////////////////////////////////

// TODO: Delete this, and this file, after templatizing state machines
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
