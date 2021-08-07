#include "stdafx.h"
#include "Viewport.h"

#include "core_math/AABB4.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

void Viewport::SlideToFit( Coord pos, CoordElem xMargin, CoordElem yMargin )
{
	RF_ASSERT( xMargin >= 0 );
	RF_ASSERT( yMargin >= 0 );
	Vec2 const marginVec = { xMargin, yMargin };
	AABB const bounds = {
		mOffset + marginVec,
		mOffset + mSurfaceExtents - marginVec };

	CoordElem const distToLeft = bounds.Left() - pos.x;
	CoordElem const distToRight = bounds.Right() - pos.x;
	if( distToLeft > 0 )
	{
		mOffset.x -= distToLeft;
	}
	else if( distToRight < 0 )
	{
		mOffset.x -= distToRight;
	}

	CoordElem const distToTop = bounds.Top() - pos.y;
	CoordElem const distToBottom = bounds.Bottom() - pos.y;
	if( distToTop > 0 )
	{
		mOffset.y -= distToTop;
	}
	else if( distToBottom < 0 )
	{
		mOffset.y -= distToBottom;
	}
}



void Viewport::ClampToWithin( AABB aabb )
{
	mOffset.x = math::Clamp<CoordElem>( aabb.Left(), mOffset.x, aabb.Right() - mSurfaceExtents.x );
	mOffset.y = math::Clamp<CoordElem>( aabb.Top(), mOffset.y, aabb.Bottom() - mSurfaceExtents.y );
}

///////////////////////////////////////////////////////////////////////////////
}
