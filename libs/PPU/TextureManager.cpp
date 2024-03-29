#include "stdafx.h"
#include "TextureManager.h"

#include "PPU/Texture.h"
#include "PPU/DeviceInterface.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"
#include "Logging/Logging.h"
#include "Allocation/AccessorDeclaration.h"

#include "core_allocate/DefaultAllocCreator.h"

#include "core/ptr/default_creator.h"


namespace RF::gfx {
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
	UniquePtr<Texture> newTexture = alloc::DefaultAllocCreator<Texture>::Create( *alloc::GetAllocator<RFTAG_PPU>() );

	// Loading deferred to post-load step
	return newTexture;
}



bool TextureManager::PostLoadFromFile( ResourceType& resource, Filename filename )
{
	if( mDeviceInterface != nullptr )
	{
		bool const successfulLoad = LoadToDevice( resource, filename );
		RFLOG_TEST_AND_NOTIFY( successfulLoad, filename, RFCAT_PPU, "Failed to load to device" );
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
	RF_TODO_BREAK();
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

bool TextureManager::LoadToDevice( ResourceType& resource, Filename const& filename )
{
	RF_ASSERT( mDeviceInterface != nullptr );
	RF_ASSERT( resource.mDeviceRepresentation == kInvalidDeviceTextureID );
	file::VFS* const vfs = mVfs;
	file::FileHandlePtr fileHandle = vfs->GetFileForRead( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to load file" );
		return false;
	}
	file::FileBuffer const buffer{ *fileHandle.Get(), false };
	RF_ASSERT( buffer.IsEmpty() == false );
	resource.mDeviceRepresentation = mDeviceInterface->LoadTexture( buffer.GetBytes(), resource.mWidthPostLoad, resource.mHeightPostLoad );
	RF_ASSERT( resource.mDeviceRepresentation != kInvalidDeviceTextureID );
	resource.UpdateFrameUsage();
	return true;
}



bool TextureManager::UnloadFromDevice( ResourceType& resource )
{
	RF_ASSERT( mDeviceInterface != nullptr );
	RF_ASSERT( resource.mDeviceRepresentation != kInvalidDeviceTextureID );
	bool const retVal = mDeviceInterface->UnloadTexture( resource.mDeviceRepresentation );
	resource.mDeviceRepresentation = kInvalidDeviceTextureID;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
