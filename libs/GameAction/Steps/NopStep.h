#pragma once
#include "project.h"

#include "GameAction/Step.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Performs no action
class GAMEACTION_API NopStep : public Step
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	virtual UniquePtr<Context> Execute( Environment const& env, Context& ctx ) const override;
};

///////////////////////////////////////////////////////////////////////////////
}
