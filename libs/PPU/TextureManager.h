#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/ResourceManager.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/unordered_map"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API TextureManager final : public ResourceManager<Texture, ManagedTextureID, k_InvalidManagedTextureID>
{
	RF_NO_COPY( TextureManager );

	//
	// Public methods
public:
	TextureManager( WeakPtr<file::VFS> const& vfs );
	~TextureManager();

	bool AttachToDevice( WeakPtr<DeviceInterface> const& deviceInterface );


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( Filename const & filename ) override;
	virtual bool PostLoadFromFile( ResourceType& resource, Filename filename ) override;
	virtual bool PostLoadFromMemory( ResourceType& resource ) override;
	virtual bool PreDestroy( ResourceType& resource ) override;


	//
	// Private methods
private:
	bool LoadToDevice( Texture& texture, Filename const& filename );
	bool UnloadFromDevice( Texture& texture );


	//
	// Private data
private:
	WeakPtr<file::VFS> const mVfs;
	WeakPtr<DeviceInterface> m_DeviceInterface;
};

///////////////////////////////////////////////////////////////////////////////
}}
