#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/macros.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API TileLayerCSVLoader
{
	RF_NO_INSTANCE( TileLayerCSVLoader );

public:
	static bool LoadTiles( TileLayer& tileLayer, file::VFS const& vfs, file::VFSPath const& path );
};

///////////////////////////////////////////////////////////////////////////////
}
