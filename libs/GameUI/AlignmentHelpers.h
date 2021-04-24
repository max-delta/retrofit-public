#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

gfx::PPUCoord GAMEUI_API AlignToJustify(
	gfx::PPUCoord objectDimensions,
	gfx::AABB const& enclosure,
	Justification justification );

///////////////////////////////////////////////////////////////////////////////
}
