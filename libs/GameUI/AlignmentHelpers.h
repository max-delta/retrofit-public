#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

gfx::ppu::Coord GAMEUI_API AlignToJustify(
	gfx::ppu::Coord objectDimensions,
	gfx::ppu::AABB const& enclosure,
	Justification justification );

///////////////////////////////////////////////////////////////////////////////
}
