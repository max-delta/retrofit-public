#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

#include "core_state/VariableIdentifier.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Non-binding, immutable
class Meta final : public state::NonBindingComponent
{
	RF_NO_COPY( Meta );

	//
	// Public methods
public:
	Meta() = delete;
	Meta( state::VariableIdentifier const& identifier );


	//
	// Public data
public:
	state::VariableIdentifier const mIdentifier;
};

///////////////////////////////////////////////////////////////////////////////
}
