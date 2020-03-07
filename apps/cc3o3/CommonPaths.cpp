#include "stdafx.h"
#include "CommonPaths.h"

#include "PlatformFilesystem/VFS.h"


namespace RF { namespace cc { namespace paths {
///////////////////////////////////////////////////////////////////////////////

file::VFSPath VfsConfig()
{
	return file::VFS::kRoot.GetChild( "config", "vfs_cc3o3.ini" );
}



file::VFSPath FontsRoot()
{
	return file::VFS::kRoot.GetChild( "assets", "fonts" );
}
file::VFSPath TilesetsRoot()
{
	return file::VFS::kRoot.GetChild( "assets", "tilesets" );
}
file::VFSPath TilemapsRoot()
{
	return file::VFS::kRoot.GetChild( "assets", "tilemaps" );
}
file::VFSPath FramepacksRoot()
{
	return file::VFS::kRoot.GetChild( "assets", "framepacks" );
}
file::VFSPath TablesRoot()
{
	return file::VFS::kRoot.GetChild( "assets", "tables" );
}

file::VFSPath CommonFonts()
{
	return file::VFS::kRoot.GetChild( "assets", "fonts", "common" );
}
file::VFSPath CommonTilesets()
{
	return file::VFS::kRoot.GetChild( "assets", "tilesets", "common" );
}



file::VFSPath BackgroundTilesets()
{
	return file::VFS::kRoot.GetChild( "assets", "tilesets", "backgrounds" );
}
file::VFSPath BackgroundTilemaps()
{
	return file::VFS::kRoot.GetChild( "assets", "tilemaps", "backgrounds" );
}
file::VFSPath LogoFramepacks()
{
	return file::VFS::kRoot.GetChild( "assets", "framepacks", "logos" );
}
file::VFSPath CharacterTables()
{
	return file::VFS::kRoot.GetChild( "assets", "tables", "char" );
}
file::VFSPath CharacterPieces()
{
	return file::VFS::kRoot.GetChild( "assets", "textures", "char" );
}
file::VFSPath Localizations()
{
	return file::VFS::kRoot.GetChild( "assets", "localizations" );
}
file::VFSPath BaseCharacters()
{
	return file::VFS::kRoot.GetChild( "assets", "tables", "world", "chars" );
}
file::VFSPath CompositeCharacters()
{
	return file::VFS::kRoot.GetChild( "scratch", "composite_char" );
}
file::VFSPath ElementDescTables()
{
	return file::VFS::kRoot.GetChild( "assets", "tables", "elements", "descs" );
}
file::VFSPath ElementTierUnlockTables()
{
	return file::VFS::kRoot.GetChild( "assets", "tables", "elements", "tier_unlocks" );
}

///////////////////////////////////////////////////////////////////////////////
}}}
