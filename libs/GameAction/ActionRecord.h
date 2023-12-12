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
	// Public data
public:
	// NOTE: Not intended to be accessed directly, except by reflection
	RF_TODO_ANNOTATION( "Need a better approach for this" );
	UniquePtr<Step> mRootStepInternal = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
