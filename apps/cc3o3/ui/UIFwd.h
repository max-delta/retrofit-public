#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

namespace RF { namespace cc { namespace ui {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::ui;

namespace font {
enum : FontPurposeID
{
	Invalid = kInvalidFontPurposeID,
	MinSize,
	NarrowTileMono,
	SmallMenuSelection,
	LargeMenuSelection,
	LargeMenuHeader
};
}

namespace focusevent {
using namespace RF::ui::focusevent;
enum : FocusEventType
{
	UNUSED = BEGIN_CUSTOM_TYPES,

	Command_ActivateCurrentFocus,
	Command_CancelCurrentFocus
};
}

///////////////////////////////////////////////////////////////////////////////
}}}
