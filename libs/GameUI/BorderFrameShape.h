#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "core_math/Vector2.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

// Defines the shape of a a border frame, making it easier to conventionalize
//  them to common shapes
// NOTE: Frame tilesets are laid out in varying sizes with this pattern:
//  / - \
//  | # |
//  \ - /
// NOTE: The interior space '#' and edges '|' or '-' may be multiple tiles
struct BorderFrameShape
{
	RF_DEFAULT_EQUALS( BorderFrameShape );

public:

	// How big a tile is, in texels
	gfx::ppu::Coord mExpectedTileDimensions;

	// For the whole pattern, how big it is, in texels
	gfx::ppu::Coord mExpectedPatternDimensions;

	// How many texels in does the border conceptually end and content can
	//  begin, in texels
	// NOTE: This is RELATIVE to the edge and corner tiles, which are assumed
	//  to consume all of the space by default, so it may be that for some
	//  frames a negative padding makes sense, to reclaim space taken by the
	//  outer tiles
	gfx::ppu::Coord mPaddingDimensions;

	// Adds additional padding as desired
	constexpr BorderFrameShape AddPadding( gfx::ppu::Coord const& padding ) const
	{
		BorderFrameShape retVal = *this;
		retVal.mPaddingDimensions += padding;
		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}
