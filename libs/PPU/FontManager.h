#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "PPUFwd.h"
#include "PPU/ResourceManager.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FontManager final : public ResourceManager<Font, ManagedFontID, kInvalidManagedFontID>
{
	RF_NO_COPY( FontManager );

	//
	// Public methods
public:
	FontManager() = delete;
	explicit FontManager( WeakPtr<file::VFS> const& vfs );
	virtual ~FontManager() override;

	bool AttachToDevice( WeakPtr<DeviceInterface> const& deviceInterface );


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( Filename const& filename ) override;
	virtual bool PostLoadFromFile( ResourceType& resource, Filename filename ) override;
	virtual bool PostLoadFromMemory( ResourceType& resource ) override;


	//
	// Private methods
private:
	bool LoadToDevice( ResourceType& resource, Filename const& filename );


	// Private data
private:
	WeakPtr<file::VFS> const mVfs;
	WeakPtr<DeviceInterface> mDeviceInterface;
};

///////////////////////////////////////////////////////////////////////////////
}
