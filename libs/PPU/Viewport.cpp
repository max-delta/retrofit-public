#include "stdafx.h"
#include "Viewport.h"

#include "core_math/AABB4.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

void Viewport::SlideToFit( PPUCoord pos, PPUCoordElem xMargin, PPUCoordElem yMargin )
{
	RF_ASSERT( xMargin >= 0 );
	RF_ASSERT( yMargin >= 0 );
	PPUVec const marginVec = { xMargin, yMargin };
	math::AABB4<PPUCoordElem> const bounds = {
		mOffset + marginVec,
		mOffset + mSurfaceExtents - marginVec };

	PPUCoordElem const distToLeft = bounds.Left() - pos.x;
	PPUCoordElem const distToRight = bounds.Right() - pos.x;
	if( distToLeft > 0 )
	{
		mOffset.x -= distToLeft;
	}
	else if( distToRight < 0 )
	{
		mOffset.x -= distToRight;
	}

	PPUCoordElem const distToTop = bounds.Top() - pos.y;
	PPUCoordElem const distToBottom = bounds.Bottom() - pos.y;
	if( distToTop > 0 )
	{
		mOffset.y -= distToTop;
	}
	else if( distToBottom < 0 )
	{
		mOffset.y -= distToBottom;
	}
}



void Viewport::ClampToWithin( math::AABB4<PPUCoordElem> aabb )
{
	mOffset.x = math::Clamp<PPUCoordElem>( aabb.Left(), mOffset.x, aabb.Right() - mSurfaceExtents.x );
	mOffset.y = math::Clamp<PPUCoordElem>( aabb.Top(), mOffset.y, aabb.Bottom() - mSurfaceExtents.y );
}

///////////////////////////////////////////////////////////////////////////////
}
