#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API FocusEvent
{
	RF_NO_COPY( FocusEvent );

	//
	// Public methods
public:
	FocusEvent() = delete;
	FocusEvent( FocusEventType eventType );


	//
	// Public data
public:
	FocusEventType mEventType = focusevent::Invalid;
};

///////////////////////////////////////////////////////////////////////////////
}
