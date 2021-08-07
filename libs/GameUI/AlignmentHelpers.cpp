#include "stdafx.h"
#include "AlignmentHelpers.h"

#include "GameUI/Font.h"

#include "PPU/PPUController.h"

#include "core_math/AABB4.h"
#include "core_math/Lerp.h"
#include "core_math/BitwiseEnums.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

gfx::ppu::Coord AlignToJustify(
	gfx::ppu::Vec2 objectDimensions,
	gfx::ppu::AABB const& enclosure,
	Justification::Value justification )
{
	using namespace enable_bitwise_enums;

	RF_ASSERT( enclosure.Left() <= enclosure.Right() );
	RF_ASSERT( enclosure.Top() <= enclosure.Bottom() );

	// Fallback
	gfx::ppu::Coord pos = enclosure.mTopLeft;

	// Y
	if( justification & Justification::Top )
	{
		pos.y = enclosure.Top();
	}
	else if( justification & Justification::Bottom )
	{
		pos.y = enclosure.Bottom() - objectDimensions.y;
	}
	else if( justification & Justification::Middle )
	{
		gfx::ppu::CoordElem const top = enclosure.Top();
		gfx::ppu::CoordElem const bottom = enclosure.Bottom() - objectDimensions.y;
		pos.y = math::Lerp( top, bottom, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	// X
	if( justification & Justification::Left )
	{
		pos.x = enclosure.Left();
	}
	else if( justification & Justification::Right )
	{
		pos.x = enclosure.Right() - objectDimensions.x;
	}
	else if( justification & Justification::Center )
	{
		gfx::ppu::CoordElem const left = enclosure.Left();
		gfx::ppu::CoordElem const right = enclosure.Right() - objectDimensions.x;
		pos.x = math::Lerp( left, right, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	return pos;
}



gfx::ppu::Coord GAMEUI_API AlignToJustifyAroundPoint(
	gfx::ppu::Vec2 objectDimensions,
	gfx::ppu::Coord point,
	Justification::Value justification )
{
	using namespace enable_bitwise_enums;

	// Will use AABB logic with a zero-sized AABB, so need to invert
	//  justifications to get correct results

	// Y
	if( justification & Justification::Top )
	{
		justification &= ~Justification::Top;
		justification |= Justification::Bottom;
	}
	else if( justification & Justification::Bottom )
	{
		justification &= ~Justification::Bottom;
		justification |= Justification::Top;
	}

	// X
	if( justification & Justification::Left )
	{
		justification &= ~Justification::Left;
		justification |= Justification::Right;
	}
	else if( justification & Justification::Right )
	{
		justification &= ~Justification::Right;
		justification |= Justification::Left;
	}

	return AlignToJustify( objectDimensions, { point, point }, justification );
}



gfx::ppu::Vec2 GAMEUI_API CalculatePrimaryFontExtents(
	gfx::ppu::PPUController const& renderer,
	Font const& font,
	char const* text )
{
	return CalculatePrimaryFontExtents( renderer, font.mManagedFontID, font.mFontHeight, font.mBaselineOffset, text );
}



gfx::ppu::Vec2 GAMEUI_API CalculatePrimaryFontExtents(
	gfx::ppu::PPUController const& renderer,
	gfx::ManagedFontID managedFontID,
	uint8_t fontHeight,
	uint8_t baselineOffset,
	char const* text )
{
	gfx::ppu::CoordElem const stringWidth = renderer.CalculateStringLength( fontHeight, managedFontID, text );

	// Push the descenders down below the baseline
	//  (tails like g, j, p, q, y )
	gfx::ppu::CoordElem const effectiveHeight = math::integer_cast<gfx::ppu::CoordElem>( fontHeight - baselineOffset );

	return { stringWidth, effectiveHeight };
}

///////////////////////////////////////////////////////////////////////////////
}
