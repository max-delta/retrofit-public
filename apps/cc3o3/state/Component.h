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
	//  bind their rollback-sensitive variables as needed and unbind during
	//  their destruction
	// NOTE: Use 'RF::rollback::AutoVar<...>' to automate much of this
	// NOTE: BindToMeta(...) assumes the presence of a meta component that
	//  contains the variable identifier
	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent );
	void BindToMeta( Window& sharedWindow, Window& privateWindow, ObjectRef const& ref );


	//
	// Protected methods
protected:
	Component() = default;
};

///////////////////////////////////////////////////////////////////////////////

// Some components do not need to bind, such as:
//  * Output-only components
//  * Immutable components
//  * State-less components
class NonBindingComponent : public Component
{
public:
	virtual void Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent ) override final;
};

///////////////////////////////////////////////////////////////////////////////
}
