#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/macros.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PaletteLoader
{
	RF_NO_INSTANCE( PaletteLoader );

public:
	static bool LoadPalette( Palette4a5_16& palette, file::VFS const& vfs, file::VFSPath const& path );
};

///////////////////////////////////////////////////////////////////////////////
}
