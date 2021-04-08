#pragma once
#include "core_math/Vector2.h"


namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////

namespace Direction {
enum Value : uint8_t
{
	Invalid = 0,

	North = 1 << 0,
	East = 1 << 1,
	South = 1 << 2,
	West = 1 << 3,

	NE = North | East,
	SE = South | East,
	NW = North | West,
	SW = South | West,
};
}


// Coordinates are +X=left, +Y=down
using PhysCoordElem = int16_t;
using PhysCoord = math::Vector2<PhysCoordElem>;

///////////////////////////////////////////////////////////////////////////////
}
