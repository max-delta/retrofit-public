#pragma once
#include "project.h"

#include "core_math/BitwiseEnums.h"

// TODO: Remove
#include "core_math/Vector2.h"

// Forwards
namespace RF::math {
template<typename T>
class Vector2;
template<typename T>
class AABB4;
}

namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////

namespace Direction {
using namespace enable_bitwise_enums;
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

static constexpr Value kCardinals[4] = { North, East, South, West };
static constexpr Value kInterCardinals[4] = { NE, SE, NW, SW };
static constexpr Value kConflicts[2] = { North | South, East | West };
}


// Coordinates are +X=left, +Y=down
using PhysCoordElem = int16_t;
using PhysCoord = math::Vector2<PhysCoordElem>;
using PhysVec = math::Vector2<PhysCoordElem>;
using AABB = math::AABB4<PhysCoordElem>;

///////////////////////////////////////////////////////////////////////////////
}
