#pragma once
#include "project.h"

#include "GamePixelPhysics/PhysicsFwd.h"

#include "core/macros.h"


namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////

// NOTE: General ordering of parameters is that more complex types are first
class GAMEPIXELPHYSICS_API PrimitiveCollision
{
	RF_NO_INSTANCE( PrimitiveCollision );

	//
	// Public methods
public:
	static bool HasCollision(
		AABB const& aabb,
		Coord const& pos );
};

///////////////////////////////////////////////////////////////////////////////
}
