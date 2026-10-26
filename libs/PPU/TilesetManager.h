#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "PPU/PPUFwd.h"
#include "PPU/ResourceManager.h"

#include "rftl/functional"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API TilesetManager final : public ResourceManager<Tileset, ManagedTilesetID, kInvalidManagedTilesetID>
{
	RF_NO_COPY( TilesetManager );

	//
	// Types and constants
public:
	using TextureLoadRefSig = ManagedTextureID( ResourceNameView resourceName, file::VFSPath const& path );
	using TextureLoadRefFunc = rftl::function<TextureLoadRefSig>;
	using TextureDestroyRefSig = bool( ResourceNameView resourceName );
	using TextureDestroyRefFunc = rftl::function<TextureDestroyRefSig>;


	//
	// Public methods
public:
	TilesetManager() = delete;
	explicit TilesetManager(
		WeakPtr<file::VFS> const& vfs,
		TextureLoadRefFunc&& texLoadFunc,
		TextureDestroyRefFunc&& texDestroyFunc );
	virtual ~TilesetManager() override;


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( ResourceNameView resourceName, Filename const& filename ) override;
	virtual bool PreDestroy( ResourceNameView resourceName, ResourceType& resource ) override;


	//
	// Private data
private:
	WeakPtr<file::VFS> const mVfs;
	TextureLoadRefFunc mTexLoadFunc;
	TextureDestroyRefFunc mTexDestroyFunc;
};

///////////////////////////////////////////////////////////////////////////////
}
