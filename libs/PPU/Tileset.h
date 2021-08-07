#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core/macros.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Tileset
{
	RF_NO_COPY( Tileset );

private:
	friend class ppu::PPUController;
	friend class TilesetManager;


private:
	using TileSize = uint8_t;
	static constexpr TileSize kDefaultTileSize = 32;


public:
	Tileset();
	Tileset( Tileset&& ) = default;
	~Tileset();
	Tileset& operator=( Tileset&& ) = default;

	uint32_t DebugGetWidth() const;
	uint32_t DebugGetHeight() const;


private:
	friend class TilesetManager;
	ManagedTextureID mTextureReference = kInvalidManagedTextureID;
	TileSize mTileWidth = kDefaultTileSize;
	TileSize mTileHeight = kDefaultTileSize;
};

///////////////////////////////////////////////////////////////////////////////
}
