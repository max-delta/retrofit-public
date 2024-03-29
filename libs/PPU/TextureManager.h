#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"
#include "PPU/ResourceManager.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/unordered_map"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API TextureManager final : public ResourceManager<Texture, ManagedTextureID, kInvalidManagedTextureID>
{
	RF_NO_COPY( TextureManager );

	//
	// Public methods
public:
	TextureManager( WeakPtr<file::VFS> const& vfs );
	virtual ~TextureManager() override;

	bool AttachToDevice( WeakPtr<DeviceInterface> const& deviceInterface );


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( Filename const& filename ) override;
	virtual bool PostLoadFromFile( ResourceType& resource, Filename filename ) override;
	virtual bool PostLoadFromMemory( ResourceType& resource ) override;
	virtual bool PreDestroy( ResourceType& resource ) override;


	//
	// Private methods
private:
	bool LoadToDevice( ResourceType& resource, Filename const& filename );
	bool UnloadFromDevice( ResourceType& resource );


	//
	// Private data
private:
	WeakPtr<file::VFS> const mVfs;
	WeakPtr<DeviceInterface> mDeviceInterface;
};

///////////////////////////////////////////////////////////////////////////////
}
