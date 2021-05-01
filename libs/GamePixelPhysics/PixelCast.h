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
		Coord const& pos );

	// If a position is out of bounds, try to recover it
	// NOTE: Dangerous, any call to this is a potential exploit or a soft-lock,
	//  prefer alternate methods during design-time to permit player recovery
	static Coord FixOutOfBounds(
		math::Bitmap const& collisionMap,
		Coord const& pos );

	// Attempt to step a pixel in a direction, under the assumption it is
	//  under user control (and so should be lenient but intuitive)
	// NOTE: Cardinal step is more restrictive on cardinal movement
	static Coord CardinalStepCast(
		math::Bitmap const& collisionMap,
		Coord const& pos,
		Direction::Value direction );
	static Coord SlideStepCast(
		math::Bitmap const& collisionMap,
		Coord const& pos,
		Direction::Value direction );
};

///////////////////////////////////////////////////////////////////////////////
}
