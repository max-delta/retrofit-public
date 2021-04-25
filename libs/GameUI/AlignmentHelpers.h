#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

gfx::ppu::PPUCoord GAMEUI_API AlignToJustify(
	gfx::ppu::PPUCoord objectDimensions,
	gfx::ppu::AABB const& enclosure,
	Justification justification );

///////////////////////////////////////////////////////////////////////////////
}
