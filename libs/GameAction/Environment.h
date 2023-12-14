#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Stores information about the execution environment of the action system,
//  such as databases of operations that can be performed
class GAMEACTION_API Environment final
{
	//
	// Public methods
public:
	Environment() = default;


	//
	// Public data
public:
	WeakPtr<ActionDatabase const> mActionDatabase = nullptr;
	WeakPtr<ConditionDatabase const> mConditionDatabase = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
