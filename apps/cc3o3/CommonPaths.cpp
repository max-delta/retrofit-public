#include "stdafx.h"
#include "CommonPaths.h"

#include "PlatformFilesystem/VFS.h"


namespace RF::cc::paths {
///////////////////////////////////////////////////////////////////////////////

file::VFSPath VfsConfig()
{
	return file::VFS::kRoot.GetChild( "config", "vfs_cc3o3.ini" );
}



file::VFSPath UserSavesRoot()
{
	return file::VFS::kRoot.GetChild( "saves" );
}



file::VFSPath AssetsRoot()
{
	return file::VFS::kRoot.GetChild( "assets" );
}

///////////////////////////////////////////////////////////////////////////////


file::VFSPath FontsRoot()
{
	return AssetsRoot().GetChild( "fonts" );
}
file::VFSPath TilesetsRoot()
{
	return AssetsRoot().GetChild( "tilesets" );
}
file::VFSPath TilemapsRoot()
{
	return AssetsRoot().GetChild( "tilemaps" );
}
file::VFSPath FramepacksRoot()
{
	return AssetsRoot().GetChild( "framepacks" );
}
file::VFSPath TexturesRoot()
{
	return AssetsRoot().GetChild( "textures" );
}
file::VFSPath TablesRoot()
{
	return AssetsRoot().GetChild( "tables" );
}

///////////////////////////////////////////////////////////////////////////////


file::VFSPath CommonFonts()
{
	return FontsRoot().GetChild( "common" );
}

///////////////////////////////////////////////////////////////////////////////

file::VFSPath CommonTilesets()
{
	return TilesetsRoot().GetChild( "common" );
}
file::VFSPath BackgroundTilesets()
{
	return TilesetsRoot().GetChild( "backgrounds" );
}
file::VFSPath UITilesets()
{
	return TilesetsRoot().GetChild( "ui" );
}
file::VFSPath DialogFrameTilesets()
{
	return TilesetsRoot().GetChild( "frames" );
}

///////////////////////////////////////////////////////////////////////////////

file::VFSPath BackgroundTilemaps()
{
	return TilemapsRoot().GetChild( "backgrounds" );
}

///////////////////////////////////////////////////////////////////////////////

file::VFSPath LogoFramepacks()
{
	return FramepacksRoot().GetChild( "logos" );
}

///////////////////////////////////////////////////////////////////////////////

file::VFSPath CharacterPieces()
{
	return TexturesRoot().GetChild( "char" );
}

///////////////////////////////////////////////////////////////////////////////

file::VFSPath CharacterTables()
{
	return TablesRoot().GetChild( "char" );
}
file::VFSPath BaseCharacters()
{
	return TablesRoot().GetChild( "world", "chars" );
}
file::VFSPath ElementDescTables()
{
	return TablesRoot().GetChild( "elements", "descs" );
}
file::VFSPath ElementTierUnlockTables()
{
	return TablesRoot().GetChild( "elements", "tier_unlocks" );
}

///////////////////////////////////////////////////////////////////////////////

file::VFSPath Localizations()
{
	return AssetsRoot().GetChild( "localizations" );
}
file::VFSPath CompositeCharacters()
{
	return file::VFS::kRoot.GetChild( "scratch", "composite_char" );
}

///////////////////////////////////////////////////////////////////////////////
}
