#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "PPUFwd.h"
#include "PPU/ResourceManager.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API TilesetManager final : public ResourceManager<Tileset, ManagedTilesetID, kInvalidManagedTilesetID>
{
	RF_NO_COPY( TilesetManager );

	//
	// Public methods
public:
	TilesetManager() = delete;
	explicit TilesetManager( WeakPtr<gfx::TextureManager> const& texMan, WeakPtr<file::VFS> const& vfs );
	virtual ~TilesetManager() override;


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
}}
