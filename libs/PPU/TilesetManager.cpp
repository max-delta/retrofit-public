#include "stdafx.h"
#include "TilesetManager.h"

#include "PPU/Tileset.h"
#include "PlatformFilesystem/VFS.h"
#include "Allocation/AccessorDeclaration.h"

#include "core_allocate/DefaultAllocCreator.h"
#include "core_math/math_casts.h"
#include "core_vfs/FileBuffer.h"
#include "core_vfs/SeekHandle.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/string_parse.h"


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
	file::SeekHandlePtr fileHandle = vfs.GetFileForRead( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to open tileset file" );
		return nullptr;
	}

	// Read into buffer
	file::FileBuffer buffer( *fileHandle, false );

	// Close
	fileHandle = nullptr;

	// Values that need to be deserialized
	uint64_t tileWidth = 0;
	uint64_t tileHeight = 0;
	rftl::string_view pathString;

	// Deserialize
	size_t lineNumber = 0;
	rftl::string_view const rawText = buffer.GetChars();
	rftl::string_view parser = rawText;
	while( parser.empty() == false )
	{
		// NOTE: One-indexed
		lineNumber++;
		RF_ASSERT_MSG( lineNumber > 0, "Rollover" );

		// Extract next line
		rftl::string_view const line = rftl::strtok_view( parser, '\n' );

		// Expect everything to be of the form '? ...'
		if( line.size() < 3 )
		{
			RFLOG_ERROR(
				filename, RFCAT_PPU,
				"Entry too short at line #{}: {}",
				lineNumber, line );
			return nullptr;
		}
		char const command = line.front();
		if( line.at( 1 ) != ' ' )
		{
			RFLOG_ERROR(
				filename, RFCAT_PPU,
				"Missing space after command at line #{}: {}",
				lineNumber, line );
			return nullptr;
		}
		rftl::string_view const params = line.substr( 2 );

		// Handle the command
		switch( command )
		{
			case 'w':
			{
				// Width
				if( lineNumber != 1 )
				{
					RFLOG_ERROR(
						filename, RFCAT_PPU,
						"Command '{}' must be first line, found at line #{}: {}",
						command, lineNumber, line );
					return nullptr;
				}

				bool const parsed = rftl::parse_int( tileWidth, params );
				if( parsed == false )
				{
					RFLOG_ERROR(
						filename, RFCAT_PPU,
						"Could not parse integer, found at line #{}: {}",
						lineNumber, line );
					return nullptr;
				}

				break;
			}
			case 'h':
			{
				// Height
				if( lineNumber != 2 )
				{
					RFLOG_ERROR(
						filename, RFCAT_PPU,
						"Command '{}' must be second line, found at line #{}: {}",
						command, lineNumber, line );
					return nullptr;
				}

				bool const parsed = rftl::parse_int( tileHeight, params );
				if( parsed == false )
				{
					RFLOG_ERROR(
						filename, RFCAT_PPU,
						"Could not parse integer, found at line #{}: {}",
						lineNumber, line );
					return nullptr;
				}

				break;
			}
			case 'f':
			{
				// File
				if( lineNumber != 3 )
				{
					RFLOG_ERROR(
						filename, RFCAT_PPU,
						"Command '{}' must be third line, found at line #{}: {}",
						command, lineNumber, line );
					return nullptr;
				}

				pathString = params;

				break;
			}
			default:
			{
				RFLOG_ERROR(
					filename, RFCAT_PPU,
					"Unsupported command '{}' at line #{}: {}",
					command, lineNumber, line );
				return nullptr;
			}
		}
	}

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
