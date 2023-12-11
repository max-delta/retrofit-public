#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Stores information about the execution environment of the action system,
//  such as databases of operations that can be performed
class GAMEACTION_API Environment final
{
	RF_NO_COPY( Environment );

	//
	// Public methods
public:
	Environment();

	ActionDatabase const& GetActionDatabase() const;
	ActionDatabase& GetMutableActionDatabase();

	ConditionDatabase const& GetConditionDatabase() const;
	ConditionDatabase& GetMutableConditionDatabase();


	//
	// Private data
private:
	UniquePtr<ActionDatabase> const mActionDatabase = nullptr;
	UniquePtr<ConditionDatabase> const mConditionDatabase = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
