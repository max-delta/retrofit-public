#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Handle to the root of a potentially large tree of checks and sub-checks,
//  depending on the complexity of the concept it represents
class GAMEACTION_API ConditionRecord final
{
	RF_NO_COPY( ConditionRecord );

	//
	// Public methods
public:
	ConditionRecord();

	UniquePtr<Check> ReplaceRoot( UniquePtr<Check>&& newRoot );
	WeakPtr<Check const> GetRoot() const;
	WeakPtr<Check> GetMutableRoot();


	//
	// Private data
private:
	UniquePtr<Check> mRootCheck = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
