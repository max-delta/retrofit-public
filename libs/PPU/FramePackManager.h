#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/ResourceManager.h"



namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackManager final : public ResourceManager<FramePackBase, ManagedFramePackID, kInvalidManagedFramePackID>
{
	RF_NO_COPY( FramePackManager );

	//
	// Public methods
public:
	FramePackManager() = delete;
	explicit FramePackManager( WeakPtr<gfx::TextureManager> const& texMan, WeakPtr<file::VFS> const& vfs );
	virtual ~FramePackManager() override;


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( Filename const& filename ) override;


	// Private data
private:
	WeakPtr<gfx::TextureManager> const mTextureManager;
	WeakPtr<file::VFS> const mVfs;
};

///////////////////////////////////////////////////////////////////////////////
}
