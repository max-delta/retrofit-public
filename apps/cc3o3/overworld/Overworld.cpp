#include "stdafx.h"
#include "Overworld.h"

#include "cc3o3/overworld/OverworldDesc.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameScripting/OOLoader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "core/ptr/default_creator.h"

#include "rftl/string_view"


namespace RF::cc::overworld {
///////////////////////////////////////////////////////////////////////////////
namespace details {

UniquePtr<OverworldDesc> LoadFromBuffer( rftl::string_view buffer )
{
	script::OOLoader loader;

	// Inject types
	{
		bool success;
		success = loader.InjectReflectedClassByCompileType<OverworldDesc>( "OverworldDesc" );
		RFLOG_TEST_AND_FATAL( success, nullptr, RFCAT_CC3O3, "Failed to inject" );
	}

	// Inject source
	{
		bool success;
		success = loader.AddSourceFromBuffer( buffer );
		RFLOG_TEST_AND_FATAL( success, nullptr, RFCAT_CC3O3, "Failed to load" );
	}

	// Extract
	UniquePtr<OverworldDesc> retVal = DefaultCreator<OverworldDesc>::Create();
	{
		bool const success = loader.PopulateClass( "map", *retVal );
		RFLOG_TEST_AND_FATAL( success, nullptr, RFCAT_CC3O3, "Failed to populate" );
	}

	return retVal;
}



UniquePtr<OverworldDesc> LoadFromFile( file::VFSPath const& filePath )
{
	file::VFS& vfs = *app::gVfs;

	// Open file
	file::FileBuffer fileBuffer( ExplicitDefaultConstruct{} );
	{
		file::FileHandlePtr const fileHandle = vfs.GetFileForRead( filePath );
		RFLOG_TEST_AND_FATAL( fileHandle != nullptr, filePath, RFCAT_CC3O3, "Failed to open overworld for read" );
		fileBuffer = file::FileBuffer( *fileHandle, true );
		size_t const fileSize = fileBuffer.GetSize();
		RFLOG_TEST_AND_FATAL( fileSize > 10, filePath, RFCAT_CC3O3, "Unreasonably small file" );
		RFLOG_TEST_AND_FATAL( fileSize < 10'000, filePath, RFCAT_CC3O3, "Unreasonably large file" );
		RFLOG_TEST_AND_FATAL( fileBuffer.GetChars().size() == fileSize, filePath, RFCAT_CC3O3, "Unexpected file size after load, expected ASCII with no nulls" );
	}

	// Load
	return LoadFromBuffer( fileBuffer.GetChars() );
}

}
///////////////////////////////////////////////////////////////////////////////

Overworld Overworld::LoadFromDesc( file::VFSPath const& descPath )
{
	Overworld retVal = {};

	UniquePtr<OverworldDesc const> descPtr = details::LoadFromFile( descPath );
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
