#include "stdafx.h"
#include "TextureManager.h"

#include "PPU/Texture.h"
#include "PPU/DeviceInterface.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
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
	RF_ASSERT_MSG( m_DeviceInterface == nullptr, "TODO: Cleanup" );

	m_DeviceInterface = deviceInterface;
	if( m_DeviceInterface != nullptr )
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
	if( m_DeviceInterface != nullptr )
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
	if( resource.m_DeviceRepresentation == k_InvalidDeviceTextureID )
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
	RF_ASSERT( m_DeviceInterface != nullptr );
	RF_ASSERT( texture.m_DeviceRepresentation == k_InvalidDeviceTextureID );
	file::VFS* const vfs = mVfs;
	file::FileHandlePtr fileHandle = vfs->GetFileForRead( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( nullptr, RFCAT_PPU, "Failed to load file" );
		return false;
	}
	FILE* fileVal = fileHandle->GetFile();
	RF_ASSERT( fileVal != nullptr );
	texture.m_DeviceRepresentation = m_DeviceInterface->LoadTexture( fileVal, texture.m_WidthPostLoad, texture.m_HeightPostLoad );
	texture.UpdateFrameUsage();
	return true;
}



bool TextureManager::UnloadFromDevice( Texture& texture )
{
	RF_ASSERT( m_DeviceInterface != nullptr );
	RF_ASSERT( texture.m_DeviceRepresentation != k_InvalidDeviceTextureID );
	bool const retVal = m_DeviceInterface->UnloadTexture( texture.m_DeviceRepresentation );
	texture.m_DeviceRepresentation = k_InvalidDeviceTextureID;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
