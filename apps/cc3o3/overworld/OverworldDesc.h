#pragma once
#include "cc3o3/overworld/OverworldFwd.h"

#include "rftl/string"


namespace RF::cc::overworld {
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
};

///////////////////////////////////////////////////////////////////////////////
}
