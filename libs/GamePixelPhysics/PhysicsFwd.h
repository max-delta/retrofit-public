#pragma once
#include "project.h"

#include "core_math/BitwiseEnums.h"


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


// Coordinates are +X=right, +Y=down (scanlines)
using CoordElem = int16_t;
using Coord = math::Vector2<CoordElem>;
using Vec2 = math::Vector2<CoordElem>;
using AABB = math::AABB4<CoordElem>;

///////////////////////////////////////////////////////////////////////////////
}
