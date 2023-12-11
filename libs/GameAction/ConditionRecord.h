#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Handle to the root of a potentially large tree of conditions and
//  sub-conditions, depending on the complexity of the concept it represents
class GAMEACTION_API ConditionRecord final
{
	RF_NO_COPY( ConditionRecord );

	//
	// Public methods
public:
	ConditionRecord();

	UniquePtr<Condition> ReplaceRoot( UniquePtr<Condition>&& newRoot );
	WeakPtr<Condition const> GetRoot() const;
	WeakPtr<Condition> GetMutableRoot();


	//
	// Private data
private:
	UniquePtr<Condition> mRootCondition = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
