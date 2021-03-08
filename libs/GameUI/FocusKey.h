#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

// This is given to focus targets when they are acknowledged by the focus
//  manager as the current owner of the focus
struct GAMEUI_API FocusKey
{
	RF_NO_COPY( FocusKey );

	//
	// Friends
private:
	friend class FocusManager;


	//
	// Public methods
public:
	FocusKey() = default;


	//
	// Private data
private:
	WeakPtr<FocusTarget> mFocusTarget;
};

///////////////////////////////////////////////////////////////////////////////
}
