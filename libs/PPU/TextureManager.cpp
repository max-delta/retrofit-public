#include "stdafx.h"
#include "TextureManager.h"

#include "PPU/Texture.h"
#include "PPU/DeviceInterface.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"
#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

TextureManager::TextureManager( WeakPtr<file::VFS> const& vfs )
	: ResourceManagerType()
	, mVfs( vfs )
{
	//
}



TextureManager::~TextureManager()
{
	InternalShutdown();
}



bool TextureManager::AttachToDevice( WeakPtr<DeviceInterface> const& deviceInterface )
{
	RF_ASSERT_MSG( mDeviceInterface == nullptr, "TODO: Cleanup" );

	mDeviceInterface = deviceInterface;
	if( mDeviceInterface != nullptr )
	{
		RF_ASSERT_MSG( GetNumResources() == 0, "TODO: Reload textures" );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

UniquePtr<TextureManager::ResourceType> TextureManager::AllocateResourceFromFile( Filename const& filename )
{
	UniquePtr<Texture> newTexture = DefaultCreator<Texture>::Create();

	// Loading deferred to post-load step
	return newTexture;
}



bool TextureManager::PostLoadFromFile( ResourceType& resource, Filename filename )
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



bool TextureManager::PostLoadFromMemory( ResourceType& resource )
{
	RF_DBGFAIL_MSG( "TODO" );
	return false;
}



bool TextureManager::PreDestroy( ResourceType& resource )
{
	if( resource.mDeviceRepresentation == kInvalidDeviceTextureID )
	{
		// Not currently in device, just toss it
		return true;
	}
	else
	{
		// In device, must unload first
		bool const unloadSuccess = UnloadFromDevice( resource );
		return unloadSuccess;
	}
}

///////////////////////////////////////////////////////////////////////////////

bool TextureManager::LoadToDevice( Texture& texture, Filename const& filename )
{
	RF_ASSERT( mDeviceInterface != nullptr );
	RF_ASSERT( texture.mDeviceRepresentation == kInvalidDeviceTextureID );
	file::VFS* const vfs = mVfs;
	file::FileHandlePtr fileHandle = vfs->GetFileForRead( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( nullptr, RFCAT_PPU, "Failed to load file" );
		return false;
	}
	file::FileBuffer const buffer{ *fileHandle.Get(), false };
	RF_ASSERT( buffer.GetData() != nullptr );
	texture.mDeviceRepresentation = mDeviceInterface->LoadTexture( buffer.GetData(), buffer.GetSize(), texture.mWidthPostLoad, texture.mHeightPostLoad );
	texture.UpdateFrameUsage();
	return true;
}



bool TextureManager::UnloadFromDevice( Texture& texture )
{
	RF_ASSERT( mDeviceInterface != nullptr );
	RF_ASSERT( texture.mDeviceRepresentation != kInvalidDeviceTextureID );
	bool const retVal = mDeviceInterface->UnloadTexture( texture.mDeviceRepresentation );
	texture.mDeviceRepresentation = kInvalidDeviceTextureID;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
