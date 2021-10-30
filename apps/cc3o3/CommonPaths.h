#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"


namespace RF::cc::paths {
///////////////////////////////////////////////////////////////////////////////

file::VFSPath VfsConfig();

file::VFSPath UserSavesRoot();
file::VFSPath AssetsRoot();

// Asset roots
file::VFSPath FontsRoot();
file::VFSPath TilesetsRoot();
file::VFSPath TilemapsRoot();
file::VFSPath FramepacksRoot();
file::VFSPath TexturesRoot();
file::VFSPath TablesRoot();

// Fonts
file::VFSPath CommonFonts();

// Tilesets
file::VFSPath CommonTilesets();
file::VFSPath BackgroundTilesets();
file::VFSPath UITilesets();
file::VFSPath DialogFrameTilesets();

// Tilemaps
file::VFSPath BackgroundTilemaps();

// Framepacks
file::VFSPath LogoFramepacks();

// Textures
file::VFSPath CharacterPieces();

// Tables
file::VFSPath Campaigns();
file::VFSPath CharacterTables();
file::VFSPath BaseCharacters();
file::VFSPath ElementDescTables();
file::VFSPath ElementTierUnlockTables();

// Misc
file::VFSPath Localizations();
file::VFSPath CompositeCharacters();

///////////////////////////////////////////////////////////////////////////////
}
