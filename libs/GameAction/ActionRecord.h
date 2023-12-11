#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Handle to the root of a potentially large tree of steps that compromise an
//  entire 'action'
class GAMEACTION_API ActionRecord final
{
	RF_NO_COPY( ActionRecord );

	//
	// Public methods
public:
	ActionRecord();

	UniquePtr<Step> ReplaceRoot( UniquePtr<Step>&& newRoot );
	WeakPtr<Step const> GetRoot() const;
	WeakPtr<Step> GetMutableRoot();


	//
	// Private data
private:
	UniquePtr<Step> mRootStep = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
