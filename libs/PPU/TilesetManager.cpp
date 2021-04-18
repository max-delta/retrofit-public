#include "stdafx.h"
#include "TilesetManager.h"

#include "PPU/Tileset.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"
#include "Allocation/AccessorDeclaration.h"

#include "core_allocate/DefaultAllocCreator.h"

#include "core/ptr/default_creator.h"

#include "rftl/sstream"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

TilesetManager::TilesetManager( WeakPtr<file::VFS> const& vfs, TextureLoadRefFunc&& texLoadFunc )
	: ResourceManagerType()
	, mVfs( vfs )
	, mTexLoadFunc( rftl::move( texLoadFunc ) )
{
	RF_ASSERT( mVfs != nullptr );
	RF_ASSERT( mTexLoadFunc != nullptr );
}



TilesetManager::~TilesetManager()
{
	InternalShutdown();
}



UniquePtr<TilesetManager::ResourceType> TilesetManager::AllocateResourceFromFile( Filename const& filename )
{
	// Open
	file::VFS const& vfs = *mVfs;
	file::FileHandlePtr fileHandle = vfs.GetFileForRead( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to open tileset file" );
		return nullptr;
	}

	// Read into buffer
	file::FileBuffer buffer( *fileHandle, false );

	// Close
	fileHandle = nullptr;

	// Deserialize
	uint64_t tileWidth = 0;
	uint64_t tileHeight = 0;
	rftl::string pathString;
	( rftl::stringstream() << buffer.GetChars().data() ) >> tileWidth >> tileHeight >> pathString;
	if( tileWidth == 0 )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to deserialize tile width" );
		return nullptr;
	}
	if( tileWidth > rftl::numeric_limits<uint8_t>::max() )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Tile width too large" );
		return nullptr;
	}
	if( tileHeight == 0 )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to deserialize tile height" );
		return nullptr;
	}
	if( tileHeight > rftl::numeric_limits<uint8_t>::max() )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Tile height too large" );
		return nullptr;
	}
	if( pathString.empty() )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to deserialize path string" );
		return nullptr;
	}
	file::VFSPath const texPath = file::VFSPath::CreatePathFromString( pathString );
	if( texPath.Empty() )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to deserialize texture path" );
		return nullptr;
	}

	ManagedTextureID const texID = mTexLoadFunc( texPath );
	if( texID == kInvalidManagedTextureID )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to load texture for tileset" );
		return nullptr;
	}

	UniquePtr<Tileset> tileset = alloc::DefaultAllocCreator<Tileset>::Create( *alloc::GetAllocator<RFTAG_PPU>() );
	tileset->mTileWidth = math::integer_cast<uint8_t>( tileWidth );
	tileset->mTileHeight = math::integer_cast<uint8_t>( tileHeight );
	tileset->mTextureReference = texID;
	return tileset;
}

///////////////////////////////////////////////////////////////////////////////
}
