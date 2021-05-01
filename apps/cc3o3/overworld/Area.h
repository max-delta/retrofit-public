#pragma once
#include "cc3o3/overworld/OverworldFwd.h"

#include "GamePixelPhysics/PhysicsFwd.h"

#include "core_math/AABB4.h"

#include "rftl/string"


namespace RF::cc::overworld {
///////////////////////////////////////////////////////////////////////////////

class Area
{
	//
	// Public data
public:
	rftl::string mIdentifier;
	AreaType mType = AreaType::Invalid;
	phys::AABB mAABB = {};
	phys::Coord mFocus = {};
};

///////////////////////////////////////////////////////////////////////////////
}
