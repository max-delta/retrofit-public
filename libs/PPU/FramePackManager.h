#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"
#include "PPU/ResourceManager.h"

#include "rftl/functional"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackManager final : public ResourceManager<FramePackBase, ManagedFramePackID, kInvalidManagedFramePackID>
{
	RF_NO_COPY( FramePackManager );

	//
	// Types and constants
public:
	using TextureLoadRefSig = ManagedTextureID( ResourceNameView resourceName, file::VFSPath const& path );
	using TextureLoadRefFunc = rftl::function<TextureLoadRefSig>;
	using TextureDestroyRefSig = bool( ResourceNameView resourceName );
	using TextureDestroyRefFunc = rftl::function<TextureDestroyRefSig>;
private:
	using TextureBackedFrames = rftl::vector<size_t>;
	using TextureBackedFramePacks = rftl::unordered_map<ManagedFramePackID, TextureBackedFrames>;


	//
	// Public methods
public:
	FramePackManager() = delete;
	explicit FramePackManager(
		WeakPtr<file::VFS> const& vfs,
		TextureLoadRefFunc&& texLoadFunc,
		TextureDestroyRefFunc&& texDestroyFunc );

	virtual ~FramePackManager() override;


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( ResourceNameView resourceName, ManagedResourceIDType managedResourceID, Filename const& filename ) override;
	virtual bool PreDestroy( ResourceNameView resourceName, ManagedResourceIDType managedResourceID, ResourceType& resource ) override;


	//
	// Private data
private:
	WeakPtr<file::VFS> const mVfs;
	TextureLoadRefFunc mTexLoadFunc;
	TextureDestroyRefFunc mTexDestroyFunc;

	TextureBackedFramePacks mTextureBackedFramePacks;
};

///////////////////////////////////////////////////////////////////////////////
}
