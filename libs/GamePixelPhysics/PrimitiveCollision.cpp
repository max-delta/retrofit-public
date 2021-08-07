#include "stdafx.h"
#include "PrimitiveCollision.h"

#include "core_math/AABB4.h"


namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////

bool PrimitiveCollision::HasCollision(
	AABB const& aabb,
	Coord const& pos )
{
	RF_ASSERT( aabb.Left() <= aabb.Right() );
	RF_ASSERT( aabb.Top() <= aabb.Bottom() );

	if( pos.x < aabb.Left() || pos.x > aabb.Right() )
	{
		return false;
	}

	if( pos.y < aabb.Top() || pos.y > aabb.Bottom() )
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
