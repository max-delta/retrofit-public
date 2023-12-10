#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Holds reusable actions that can be referenced by ID, assumed to be the
//  primary way most games get access to actions, which are presumed to be
//  loaded from disk in data-driven setups
class GAMEACTION_API ConditionDatabase final
{
	//
	// Public methods
public:
	ConditionDatabase() = default;

	// TODO: Implement


	//
	// Private data
private:
	// TODO: Implement
};

///////////////////////////////////////////////////////////////////////////////
}
