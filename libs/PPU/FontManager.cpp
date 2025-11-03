#include "stdafx.h"
#include "FontManager.h"

#include "PPU/Font.h"
#include "PPU/Texture.h"
#include "PPU/TextureManager.h"
#include "PPU/DeviceInterface.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"
#include "Allocation/AccessorDeclaration.h"

#include "core_allocate/DefaultAllocCreator.h"
#include "core_vfs/SeekHandle.h"

#include "rftl/sstream"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

FontManager::FontManager( WeakPtr<file::VFS> const& vfs )
	: ResourceManagerType()
	, mVfs( vfs )
{
	//
}



FontManager::~FontManager()
{
	InternalShutdown();
}



bool FontManager::AttachToDevice( WeakPtr<DeviceInterface> const& deviceInterface )
{
	RF_ASSERT_MSG( mDeviceInterface == nullptr, "TODO: Cleanup" );

	mDeviceInterface = deviceInterface;
	if( mDeviceInterface != nullptr )
	{
		RF_ASSERT_MSG( GetNumResources() == 0, "TODO: Reload fonts" );
	}

	return true;
}



UniquePtr<FontManager::ResourceType> FontManager::AllocateResourceFromFile( Filename const& filename )
{
	// Open
	file::VFS const& vfs = *mVfs;
	file::SeekHandlePtr fileHandle = vfs.GetFileForRead( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to open font file" );
		return nullptr;
	}

	// Read into buffer
	file::FileBuffer buffer( *fileHandle, false );

	// Close
	fileHandle = nullptr;

	// Deserialize
	rftl::string spacingModeString;
	rftl::string pathString;
	( rftl::stringstream() << buffer.GetChars().data() ) >> spacingModeString >> pathString;
	Font::SpacingMode spacingMode = Font::SpacingMode::Invalid;
	if( spacingModeString == "Fixed" )
	{
		spacingMode = Font::SpacingMode::Fixed;
	}
	else if( spacingModeString == "Variable" )
	{
		spacingMode = Font::SpacingMode::Variable;
	}
	if( spacingMode == Font::SpacingMode::Invalid )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to deserialize spacing mode" );
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

	file::SeekHandlePtr const fontHandle = vfs.GetFileForRead( texPath );
	file::FileBuffer fontBuffer{ *fontHandle.Get(), false };
	if( fontBuffer.IsEmpty() )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to load texture file for font" );
		return nullptr;
	}

	UniquePtr<Font> font = alloc::DefaultAllocCreator<Font>::Create(
		*alloc::GetAllocator<RFTAG_PPU>() );
	font->mFileBuffer = rftl::move( fontBuffer );
	font->mSpacingMode = spacingMode;
	// NOTE: Not initialized yet, will be determined on device load
	//font->mTileWidth;
	//font->mTileHeight;
	return font;
}



bool FontManager::PostLoadFromFile( ResourceType& resource, Filename filename )
{
	if( mDeviceInterface != nullptr )
	{
		bool const successfulLoad = LoadToDevice( resource, filename );
		RF_ASSERT( successfulLoad );
		return successfulLoad;
	}
	else
	{
		// Will likely be loaded on next device attach
		return true;
	}
}



bool FontManager::PostLoadFromMemory( ResourceType& resource )
{
	RF_TODO_BREAK();
	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool FontManager::LoadToDevice( ResourceType& resource, Filename const& filename )
{
	RF_ASSERT( mDeviceInterface != nullptr );
	RF_ASSERT( resource.mDeviceRepresentation == kInvalidDeviceTextureID );

	uint32_t tileWidth = 0;
	uint32_t tileHeight = 0;
	rftl::array<uint32_t, 256> variableWidth;
	rftl::array<uint32_t, 256> variableHeight;
	resource.mDeviceRepresentation = mDeviceInterface->CreateBitmapFont(
		resource.mFileBuffer.GetBytes(),
		tileWidth,
		tileHeight,
		&variableWidth,
		&variableHeight );
	RF_ASSERT( resource.mDeviceRepresentation != kInvalidDeviceTextureID );
	RF_ASSERT( tileWidth > 0 );
	RF_ASSERT( tileHeight > 0 );
	resource.mTileWidth = math::integer_cast<uint8_t>( tileWidth );
	resource.mTileHeight = math::integer_cast<uint8_t>( tileHeight );
	for( size_t i = 0; i < 256; i++ )
	{
		resource.mVariableWidth.at( i ) = math::integer_cast<Font::TileSize>( variableWidth.at( i ) );
	}
	for( size_t i = 0; i < 256; i++ )
	{
		resource.mVariableHeight.at( i ) = math::integer_cast<Font::TileSize>( variableHeight.at( i ) );
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
