#pragma once
#include "cc3o3/overworld/Area.h"

#include "PlatformFilesystem/VFSPath.h"

#include "core_math/Bitmap.h"


namespace RF::cc::overworld {
///////////////////////////////////////////////////////////////////////////////

class Overworld
{
	//
	// Types and constants
public:
	using Areas = rftl::vector<Area>;


	//
	// Public methods
public:
	Overworld() = default;

	static Overworld LoadFromDesc( file::VFSPath const& descPath );


	//
	// Public data
public:
	math::Bitmap mCollisionMap{ ExplicitDefaultConstruct{} };
	file::VFSPath mTerrainTilesetPath;
	file::VFSPath mTerrainTilemapPath;
	file::VFSPath mCloud1TilesetPath;
	file::VFSPath mCloud1TilemapPath;
	file::VFSPath mCloud2TilesetPath;
	file::VFSPath mCloud2TilemapPath;
	uint8_t mCloud1ParallaxDelay = 0;
	uint8_t mCloud2ParallaxDelay = 0;
	Areas mAreas;
};

///////////////////////////////////////////////////////////////////////////////
}
