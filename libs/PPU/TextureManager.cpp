#include "stdafx.h"
#include "TextureManager.h"

#include "PPU/Texture.h"
#include "PPU/DeviceInterface.h"

//HACK
#include <memory>

#include "core/ptr/entwined_creator.h"
#include "core/ptr/default_creator.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

TextureManager::~TextureManager()
{
	RF_ASSERT_MSG( m_Textures.empty(), "TODO: Cleanup" );
}



bool TextureManager::AttachToDevice( WeakPtr<DeviceInterface> const & deviceInterface )
{
	RF_ASSERT_MSG( m_DeviceInterface == nullptr, "TODO: Cleanup" );

	m_DeviceInterface = deviceInterface;
	if( m_DeviceInterface != nullptr )
	{
		RF_ASSERT_MSG( m_Textures.empty(), "TODO: Reload textures" );
	}

	return true;
}



void TextureManager::UpdateMostRecentFrame( FrameID frame ) const
{
	RF_ASSERT( frame > Texture::s_MostRecentFrame );
	Texture::s_MostRecentFrame = frame;
}



WeakPtr<Texture> TextureManager::GetDeviceTectureForRenderFromManagedTextureID( ManagedTextureID managedTextureID ) const
{
	TexturesByName::const_iterator iter;
	for( iter = m_Textures.begin(); iter != m_Textures.end(); iter++ )
	{
		Texture const* const texture = iter->second;

		// Use the pointer as the ID
		if( (uint64_t)texture == managedTextureID )
		{
			return iter->second;
		}
	}

	return nullptr;
}



WeakPtr<Texture> TextureManager::GetDeviceTextureForRenderFromTextureName( TextureName const & textureName ) const
{
	TexturesByName::const_iterator iter = m_Textures.find( textureName );
	if( iter == m_Textures.end() )
	{
		return nullptr;
	}

	return iter->second;
}



bool TextureManager::LoadNewTexture( TextureName const & textureName, Filename const & filename )
{
	WeakPtr<Texture> handle = LoadNewTextureGetHandle( textureName, filename );
	return handle != nullptr;
}



ManagedTextureID TextureManager::LoadNewTextureGetID( TextureName const & textureName, Filename const & filename )
{
	WeakPtr<Texture> handle = LoadNewTextureGetHandle( textureName, filename );

	// Use the pointer as the ID
	return reinterpret_cast<ManagedTextureID>( static_cast<void*>( handle ) );
}



WeakPtr<Texture> TextureManager::LoadNewTextureGetHandle( TextureName const & textureName, Filename const & filename )
{
	RF_ASSERT( textureName.empty() == false );
	RF_ASSERT( filename.empty() == false );

	RF_ASSERT( m_Textures.count( textureName ) == 0 );
	RF_ASSERT( m_FileBackedTextures.count( textureName ) == 0 );
	Texture* tex;
	WeakPtr<Texture> retVal;
	{
		UniquePtr<Texture> newTexture = DefaultCreator<Texture>::Create();
		retVal = newTexture;
		tex = newTexture;
		m_Textures.emplace( textureName, std::move( newTexture ) );
		m_FileBackedTextures.emplace( textureName, filename );
	}
	RF_ASSERT( retVal != nullptr );

	if( m_DeviceInterface != nullptr )
	{
		bool successfulLoad = LoadToDevice( *tex, filename );
		RF_ASSERT( successfulLoad );
		RF_ASSERT( retVal != nullptr );
		return retVal;
	}
	else
	{
		// Will likely be loaded on next device attach
		RF_ASSERT( retVal != nullptr );
		return retVal;
	}
}



bool TextureManager::DestroyTexture( TextureName const & textureName )
{
	TexturesByName::iterator iter = m_Textures.find( textureName );
	if( iter == m_Textures.end() )
	{
		RF_ASSERT_MSG( false, "Texture not found" );
		return false;
	}

	UniquePtr<Texture>& texture = iter->second;
	RF_ASSERT( texture != nullptr );

	if( texture->m_DeviceRepresentation == k_InvalidDeviceTextureID )
	{
		// Not currently in device, just toss it
	}
	else
	{
		// In device, must unload first
		bool unloadSuccess = UnloadFromDevice( *texture );
		RF_ASSERT( unloadSuccess );
	}

	m_FileBackedTextures.erase( textureName );
	m_Textures.erase( iter );
	return true;
}

///////////////////////////////////////////////////////////////////////////////

bool TextureManager::LoadToDevice( Texture & texture, Filename const & filename )
{
	RF_ASSERT( m_DeviceInterface != nullptr );
	RF_ASSERT( texture.m_DeviceRepresentation == k_InvalidDeviceTextureID );
	texture.m_DeviceRepresentation = m_DeviceInterface->LoadTexture( filename.c_str() );
	texture.UpdateFrameUsage();
	return true;
}



bool TextureManager::UnloadFromDevice( Texture & texture )
{
	RF_ASSERT( m_DeviceInterface != nullptr );
	RF_ASSERT( texture.m_DeviceRepresentation != k_InvalidDeviceTextureID );
	bool retVal = m_DeviceInterface->UnloadTexture( texture.m_DeviceRepresentation );
	texture.m_DeviceRepresentation = k_InvalidDeviceTextureID;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
