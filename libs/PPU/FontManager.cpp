#include "stdafx.h"
#include "FontManager.h"

#include "PPU/Font.h"
#include "PPU/Texture.h"
#include "PPU/TextureManager.h"
#include "PPU/DeviceInterface.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "core/ptr/default_creator.h"

#include "rftl/sstream"


namespace RF { namespace gfx {
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
	file::FileHandlePtr fileHandle = vfs.GetFileForRead( filename );
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
	( rftl::stringstream() << reinterpret_cast<char const*>( buffer.GetData() ) ) >> spacingModeString >> pathString;
	Font::SpacingMode spacingMode = Font::SpacingMode::Invalid;
	if( spacingModeString == "Fixed" )
	{
		spacingMode = Font::SpacingMode::Fixed;
	}
	else if( spacingModeString == "Variable" )
	{
		spacingMode = Font::SpacingMode::Variable;
	}
	if(spacingMode == Font::SpacingMode::Invalid)
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

	file::FileHandlePtr const fontHandle = vfs.GetFileForRead( texPath );
	file::FileBuffer fontBuffer{ *fontHandle.Get(), false };
	RF_ASSERT( fontBuffer.GetData() != nullptr );
	if( fontBuffer.GetSize() <= 0 )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to load texture file for font" );
		return nullptr;
	}

	UniquePtr<Font> font = DefaultCreator<Font>::Create();
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
	RF_DBGFAIL_MSG( "TODO" );
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
	resource.mDeviceRepresentation = mDeviceInterface->CreateBitmapFont(
		resource.mFileBuffer.GetData(),
		resource.mFileBuffer.GetSize(),
		tileWidth,
		tileHeight,
		&variableWidth );
	RF_ASSERT( resource.mDeviceRepresentation != kInvalidDeviceTextureID );
	RF_ASSERT( tileWidth > 0 );
	RF_ASSERT( tileHeight > 0 );
	resource.mTileWidth = math::integer_cast<uint8_t>( tileWidth );
	resource.mTileHeight = math::integer_cast<uint8_t>( tileHeight );
	resource.mVariableWidth = rftl::move( variableWidth );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}
