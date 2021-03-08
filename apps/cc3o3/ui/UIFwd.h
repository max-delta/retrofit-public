#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

namespace controller {
using namespace RF::ui::controller;
}
using namespace RF::ui;

namespace font {
enum : FontPurposeID
{
	Invalid = kInvalidFontPurposeID,
	MinSize,
	NarrowTileMono,
	NarrowHalfTileMono,
	NarrowQuarterTileMono,

	SmallMenuText,
	LargeMenuText,

	SmallMenuSelection,
	LargeMenuSelection,

	LargeMenuHeader,

	MessageBox
};
}

namespace focusevent {
using namespace RF::ui::focusevent;
enum : FocusEventType
{
	UNUSED = BEGIN_CUSTOM_TYPES,

	Command_ActivateCurrentFocus,
	Command_CancelCurrentFocus,

	Command_AuxiliaryAction1,
	Command_AuxiliaryAction2,
	Command_MenuAction,
	Command_PauseAction,
};
}


// TODO: Configurable
static constexpr uint8_t kTextSpeed = 4;

///////////////////////////////////////////////////////////////////////////////
}
