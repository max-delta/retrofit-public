#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "Rollback/RollbackFwd.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

class Component
{
	//
	// Types and constants
public:
	using Window = rollback::Window;
	using VariableIdentifier = state::VariableIdentifier;

	//
	// Public methods
public:
	virtual ~Component() = default;

	// Components are expected to participate in rollback and will be asked to
	//  bind/unbind their rollback-sensitive variables as needed
	// NOTE: Use 'RF::rollback::AutoVar<...>' to automate much of this
	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent );
	virtual void Unbind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent );


	//
	// Protected methods
protected:
	Component() = default;
};

///////////////////////////////////////////////////////////////////////////////
}
