#include "stdafx.h"
#include "Overworld.h"

#include "cc3o3/overworld/OverworldDesc.h"
#include "cc3o3/resource/ResourceLoad.h"


namespace RF::cc::overworld {
///////////////////////////////////////////////////////////////////////////////

Overworld Overworld::LoadFromDesc( file::VFSPath const& descPath )
{
	Overworld retVal = {};

	UniquePtr<OverworldDesc const> descPtr = resource::LoadFromFile<OverworldDesc const>( descPath );
	RFLOG_TEST_AND_FATAL( descPtr != nullptr, descPath, RFCAT_CC3O3, "Failed to load overworld desc" );
	OverworldDesc const& desc = *descPtr;

	retVal.mTerrainTilesetPath = file::VFSPath::CreatePathFromString( desc.mTerrainTilesetPath );
	retVal.mTerrainTilemapPath = file::VFSPath::CreatePathFromString( desc.mTerrainTilemapPath );
	retVal.mCloud1TilesetPath = file::VFSPath::CreatePathFromString( desc.mCloud1TilesetPath );
	retVal.mCloud1TilemapPath = file::VFSPath::CreatePathFromString( desc.mCloud1TilemapPath );
	retVal.mCloud2TilesetPath = file::VFSPath::CreatePathFromString( desc.mCloud2TilesetPath );
	retVal.mCloud2TilemapPath = file::VFSPath::CreatePathFromString( desc.mCloud2TilemapPath );
	retVal.mCloud1ParallaxDelay = desc.mCloud1ParallaxDelay;
	retVal.mCloud2ParallaxDelay = desc.mCloud1ParallaxDelay;

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
