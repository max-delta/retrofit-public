#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"

#include <string>
#include <unordered_map>


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API TextureManager
{
	RF_NO_COPY( TextureManager );

	//
	// Types
public:
	typedef file::VFSPath Filename;
	typedef std::string TextureName;

	typedef std::unordered_map<TextureName, UniquePtr<Texture>> TexturesByName;
	typedef std::unordered_map<TextureName, Filename> TexturesByFilename;
private:
	typedef std::pair<TexturesByFilename::const_iterator, TexturesByFilename::const_iterator> FileBackedTextureRange;


	//
	// Public methods
public:
	TextureManager() = default;
	~TextureManager();

	bool AttachToDevice( WeakPtr<DeviceInterface> const& deviceInterface );
	void UpdateMostRecentFrame( FrameID frame ) const;

	WeakPtr<Texture> GetDeviceTectureForRenderFromManagedTextureID( ManagedTextureID managedTextureID ) const;
	WeakPtr<Texture> GetDeviceTextureForRenderFromTextureName( TextureName const& textureName ) const;

	Filename GetFilenameFromTextureName( TextureName const& textureName ) const;

	bool LoadNewTexture( TextureName const& textureName, Filename const& filename );
	ManagedTextureID LoadNewTextureGetID( TextureName const& textureName, Filename const& filename );
	WeakPtr<Texture> LoadNewTextureGetHandle( TextureName const& textureName, Filename const& filename );

	bool UpdateExistingTexture( TextureName const& textureName, Filename const& filename );
	bool ReloadExistingTexture( TextureName const& textureName );
	bool DestroyTexture( TextureName const& textureName );

	std::vector<TextureName> DebugSearchForTexturesByFilename( Filename const& filename ) const;


	//
	// Private methods
private:
	bool LoadToDevice( Texture& texture, Filename const& filename );
	bool UnloadFromDevice( Texture& texture );
	FileBackedTextureRange SearchForTexturesByFilenameInternal( Filename const& filename ) const;


	//
	// Private data
private:
	WeakPtr<DeviceInterface> m_DeviceInterface;
	TexturesByName m_Textures;
	TexturesByFilename m_FileBackedTextures;
};

///////////////////////////////////////////////////////////////////////////////
}}
