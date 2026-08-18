#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/macros.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

// Reads a tile layer from CSV
// NOTE: At time of writing, the read values differ from the in-memory
//  storage of tile layets, and is mostly for compatability with common
//  tile-mapping editor software
class PPU_API TileLayerCSVLoader
{
	RF_NO_INSTANCE( TileLayerCSVLoader );

	//
	// Types and constants
public:
	using IndexType = int32_t;

	// This value indicates an empty tile
	static constexpr IndexType kEmptyTileIndex = -1;


	//
	// Public methods
public:
	static bool LoadTiles( TileLayer& tileLayer, file::VFS const& vfs, file::VFSPath const& path );
};

///////////////////////////////////////////////////////////////////////////////
}
