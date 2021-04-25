#pragma once
#include "cc3o3/overworld/OverworldFwd.h"

#include "GamePixelPhysics/PhysicsFwd.h"

#include "rftl/string"
#include "rftl/vector"


namespace RF::cc::overworld {
///////////////////////////////////////////////////////////////////////////////

struct AreaDesc
{
	rftl::string mIdentifier;
	rftl::string mType;
	phys::PhysCoordElem mX = 0;
	phys::PhysCoordElem mY = 0;
	phys::PhysCoordElem mWidth = 0;
	phys::PhysCoordElem mHeight = 0;
	phys::PhysCoordElem mFocusX = 0;
	phys::PhysCoordElem mFocusY = 0;
};

///////////////////////////////////////////////////////////////////////////////

struct OverworldDesc
{
	rftl::string mCollisionMapPath;
	rftl::string mTerrainTilesetPath;
	rftl::string mTerrainTilemapPath;
	rftl::string mCloud1TilesetPath;
	rftl::string mCloud1TilemapPath;
	rftl::string mCloud2TilesetPath;
	rftl::string mCloud2TilemapPath;
	uint8_t mCloud1ParallaxDelay = 0;
	uint8_t mCloud2ParallaxDelay = 0;

	rftl::vector<AreaDesc> mAreas;
};

///////////////////////////////////////////////////////////////////////////////
}
