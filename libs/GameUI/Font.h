#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API Font
{
	gfx::ManagedFontID mManagedFontID = gfx::kInvalidManagedFontID;
	uint8_t mFontHeight = 0;
	uint8_t mBaselineOffset = 0;
	uint8_t mMinimumZoomFactor = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
