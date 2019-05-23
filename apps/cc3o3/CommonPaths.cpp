#include "stdafx.h"
#include "CommonPaths.h"

#include "PlatformFilesystem/VFS.h"


namespace RF { namespace cc { namespace paths {
///////////////////////////////////////////////////////////////////////////////

file::VFSPath const gVfsConfig = file::VFS::kRoot.GetChild( "config", "vfs_cc3o3.ini" );

file::VFSPath const gFontsRoot = file::VFS::kRoot.GetChild( "assets", "fonts" );
file::VFSPath const gTilesetsRoot = file::VFS::kRoot.GetChild( "assets", "tilesets" );
file::VFSPath const gTilemapsRoot = file::VFS::kRoot.GetChild( "assets", "tilemaps" );
file::VFSPath const gFramepacksRoot = file::VFS::kRoot.GetChild( "assets", "framepacks" );
file::VFSPath const gTablesRoot = file::VFS::kRoot.GetChild( "assets", "tables" );

file::VFSPath const gCommonFonts = file::VFS::kRoot.GetChild( "assets", "fonts", "common" );
file::VFSPath const gCommonTilesets = file::VFS::kRoot.GetChild( "assets", "tilesets", "common" );

file::VFSPath const gBackgroundTilesets = file::VFS::kRoot.GetChild( "assets", "tilesets", "backgrounds" );
file::VFSPath const gBackgroundTilemaps = file::VFS::kRoot.GetChild( "assets", "tilemaps", "backgrounds" );
file::VFSPath const gLogoFramepacks = file::VFS::kRoot.GetChild( "assets", "framepacks", "logos" );
file::VFSPath const gCharacterTables = file::VFS::kRoot.GetChild( "assets", "tables", "char" );
file::VFSPath const gCharacterPieces = file::VFS::kRoot.GetChild( "assets", "textures", "char" );
file::VFSPath const gLocalizations = file::VFS::kRoot.GetChild( "assets", "localizations" );
file::VFSPath const gCompositeCharacters = file::VFS::kRoot.GetChild( "scratch", "composite_char" );

///////////////////////////////////////////////////////////////////////////////
}}}
