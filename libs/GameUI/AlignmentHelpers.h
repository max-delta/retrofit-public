#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "core_math/AABB4.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

gfx::PPUCoord GAMEUI_API AlignToJustify(
	gfx::PPUCoord objectDimensions,
	math::AABB4<gfx::PPUCoordElem> const& enclosure,
	Justification justification );

///////////////////////////////////////////////////////////////////////////////
}
