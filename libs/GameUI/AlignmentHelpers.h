#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

gfx::ppu::Coord GAMEUI_API AlignToJustify(
	gfx::ppu::Vec2 objectDimensions,
	gfx::ppu::AABB const& enclosure,
	Justification justification );

gfx::ppu::Coord GAMEUI_API AlignToJustifyOnPoint(
	gfx::ppu::Vec2 objectDimensions,
	gfx::ppu::Coord point,
	Justification justification );


// NOTE: Differs from naive glyph sizes, takes into account 'tails' and other
//  'baseline' concepts
gfx::ppu::Vec2 GAMEUI_API CalculatePrimaryFontExtents(
	gfx::ppu::PPUController const& renderer,
	Font const& font,
	char const* text );
gfx::ppu::Vec2 GAMEUI_API CalculatePrimaryFontExtents(
	gfx::ppu::PPUController const& renderer,
	gfx::ManagedFontID managedFontID,
	uint8_t fontHeight,
	uint8_t baselineOffset,
	char const* text );

///////////////////////////////////////////////////////////////////////////////
}
