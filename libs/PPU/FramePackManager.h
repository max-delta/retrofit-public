#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/ResourceManager.h"

#include "rftl/functional"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackManager final : public ResourceManager<FramePackBase, ManagedFramePackID, kInvalidManagedFramePackID>
{
	RF_NO_COPY( FramePackManager );

	//
	// Types and constants
public:
	using TextureLoadRefSig = ManagedTextureID( file::VFSPath const& path );
	using TextureLoadRefFunc = rftl::function<TextureLoadRefSig>;


	//
	// Public methods
public:
	FramePackManager() = delete;
	explicit FramePackManager( WeakPtr<file::VFS> const& vfs, TextureLoadRefFunc&& texLoadFunc );
	virtual ~FramePackManager() override;


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( Filename const& filename ) override;


	// Private data
private:
	WeakPtr<file::VFS> const mVfs;
	TextureLoadRefFunc mTexLoadFunc;
};

///////////////////////////////////////////////////////////////////////////////
}
