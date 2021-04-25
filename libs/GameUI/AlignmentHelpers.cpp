#include "stdafx.h"
#include "AlignmentHelpers.h"

#include "core_math/AABB4.h"
#include "core_math/Lerp.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

gfx::ppu::PPUCoord AlignToJustify(
	gfx::ppu::PPUCoord objectDimensions,
	gfx::ppu::AABB const& enclosure,
	Justification justification )
{
	// Fallback
	gfx::ppu::PPUCoord pos = enclosure.mTopLeft;

	// Y
	if( math::enum_bitcast( justification ) & math::enum_bitcast( Justification::Top ) )
	{
		pos.y = enclosure.Top();
	}
	else if( math::enum_bitcast( justification ) & math::enum_bitcast( Justification::Bottom ) )
	{
		pos.y = enclosure.Bottom() - objectDimensions.y;
	}
	else if( math::enum_bitcast( justification ) & math::enum_bitcast( Justification::Middle ) )
	{
		gfx::ppu::PPUCoordElem const top = enclosure.Top();
		gfx::ppu::PPUCoordElem const bottom = enclosure.Bottom() - objectDimensions.y;
		pos.y = math::Lerp( top, bottom, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	// X
	if( math::enum_bitcast( justification ) & math::enum_bitcast( Justification::Left ) )
	{
		pos.x = enclosure.Left();
	}
	else if( math::enum_bitcast( justification ) & math::enum_bitcast( Justification::Right ) )
	{
		pos.x = enclosure.Right() - objectDimensions.x;
	}
	else if( math::enum_bitcast( justification ) & math::enum_bitcast( Justification::Center ) )
	{
		gfx::ppu::PPUCoordElem const left = enclosure.Left();
		gfx::ppu::PPUCoordElem const right = enclosure.Right() - objectDimensions.x;
		pos.x = math::Lerp( left, right, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	return pos;
}

///////////////////////////////////////////////////////////////////////////////
}
