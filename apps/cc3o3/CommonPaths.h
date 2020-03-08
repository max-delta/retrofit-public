#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"


namespace RF { namespace cc { namespace paths {
///////////////////////////////////////////////////////////////////////////////

file::VFSPath VfsConfig();

file::VFSPath FontsRoot();
file::VFSPath TilesetsRoot();
file::VFSPath TilemapsRoot();
file::VFSPath FramepacksRoot();
file::VFSPath TablesRoot();

file::VFSPath CommonFonts();
file::VFSPath CommonTilesets();

file::VFSPath BackgroundTilesets();
file::VFSPath BackgroundTilemaps();
file::VFSPath DialogFrameTilesets();
file::VFSPath LogoFramepacks();
file::VFSPath CharacterTables();
file::VFSPath CharacterPieces();
file::VFSPath Localizations();
file::VFSPath BaseCharacters();
file::VFSPath CompositeCharacters();
file::VFSPath ElementDescTables();
file::VFSPath ElementTierUnlockTables();

///////////////////////////////////////////////////////////////////////////////
}}}
