#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"


namespace RF { namespace cc { namespace paths {
///////////////////////////////////////////////////////////////////////////////

extern file::VFSPath const gVfsConfig;

extern file::VFSPath const gFontsRoot;
extern file::VFSPath const gTilesetsRoot;
extern file::VFSPath const gTilemapsRoot;
extern file::VFSPath const gFramepacksRoot;

extern file::VFSPath const gCommonFonts;
extern file::VFSPath const gCommonTilesets;

extern file::VFSPath const gBackgroundTilesets;
extern file::VFSPath const gBackgroundTilemaps;
extern file::VFSPath const gLogoFramepacks;
extern file::VFSPath const gLocalizations;

///////////////////////////////////////////////////////////////////////////////
}}}
