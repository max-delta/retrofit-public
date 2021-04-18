#pragma once
#include "project.h"

#include "GamePixelPhysics/PhysicsFwd.h"

#include "core/macros.h"

// Forwards
namespace RF::math {
class Bitmap;
}

namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////

class GAMEPIXELPHYSICS_API PixelCast
{
	RF_NO_INSTANCE( PixelCast );

	//
	// Public methods
public:
	// NOTE: Out-of-bounds positions are treated as collisions
	static bool HasCollision(
		math::Bitmap const& collisionMap,
		PhysCoord const& pos );

	// Attempt to step a pixel in a direction, under the assumption it is
	//  under user control (and so should be lenient but intuitive)
	// NOTE: Assumes cardinals are the primary movement intent
	static PhysCoord CardinalStepCast(
		math::Bitmap const& collisionMap,
		PhysCoord const& pos,
		Direction::Value direction );
};

///////////////////////////////////////////////////////////////////////////////
}