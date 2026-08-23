#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core/macros.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

// Tilesets are a collection of tiles, intended to be referenced by tile layers
//  to create backgrounds and foregrounds
class PPU_API Tileset
{
	RF_NO_COPY( Tileset );
	RF_DEFAULT_MOVE( Tileset );

	//
	// Friends
private:
	friend class ppu::PPUController;
	friend class TilesetManager;


	//
	// Types and constants
private:
	static constexpr TileSize kDefaultTileSize = ppu::kTileSize;


	//
	// Public methods
public:
	Tileset();
	~Tileset();

	// The dimensions of a tile should not be examined by most code, as most
	//  code should just be abstractly connecting tile layers to tile sets and
	//  ignoring the implementation details
	uint32_t DebugGetTileWidth() const;
	uint32_t DebugGetTileHeight() const;


	//
	// Private data
private:
	ManagedTextureID mTextureReference = kInvalidManagedTextureID;
	TileSize mTileWidth = kDefaultTileSize;
	TileSize mTileHeight = kDefaultTileSize;
};

///////////////////////////////////////////////////////////////////////////////
}
