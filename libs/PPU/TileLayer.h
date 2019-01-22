#pragma once
#include "project.h"

#include "PPU/PPUTimer.h"

#include "core/macros.h"

#include "rftl/vector"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API TileLayer
{
	//
	// Types and constants
public:
	using TileSize = uint8_t;
	static constexpr TileSize kDefaultTileSize = 32;
	using TileIndex = int16_t;
	static constexpr TileIndex kEmptyTileIndex = -1;


	//
	// Structs
public:
	struct Tileset
	{
		ManagedTextureID mTexture = kInvalidManagedTextureID;
		TileSize mTileWidth = kDefaultTileSize;
		TileSize mTileHeight = kDefaultTileSize;
	};

	struct Tile
	{
		TileIndex mIndex = kEmptyTileIndex;
	};


	//
	// Public methods
public:
	TileLayer() = default;
	~TileLayer() = default;

	void Animate();

	size_t NumColumns() const;
	size_t NumRows() const;
	size_t NumTiles() const;

	Tile const& GetTile( size_t column, size_t row ) const;
	Tile const& GetTile( size_t index ) const;
	Tile& GetMutableTile( size_t column, size_t row );
	Tile& GetMutableTile( size_t index );

	Tile const* Data() const;
	Tile* MutableData();

	void Clear();
	void ClearAndResize( size_t columns, size_t rows );


	//
	// Public data
public:
	uint16_t mNumColumns;

	PPUCoordElem mXCoord;
	PPUCoordElem mYCoord;
	PPUDepthLayer mZLayer;

	uint8_t mGFXModifier;
	PPUTimer mTimer;

	bool mLooping : 1;
	bool mPaused : 1;
	bool mHorizontalFlip : 1;
	bool mVerticalFlip : 1;
	bool mUseTransform : 1;
	bool mWrapping : 1;
	uint8_t reserved1 : 2;
	uint8_t reserved2 : 8;
	uint8_t reserved3 : 8;
	uint8_t reserved4 : 8;

	float mTransform[2][2];

	Tileset mTileset;


	//
	// Private data
private:
	rftl::vector<Tile> mTiles;
};
static_assert( offsetof( TileLayer, mTileset ) == 32, "Double-check TileLayer storage" );
static_assert( sizeof( TileLayer ) <= 128, "Double-check TileLayer storage" );
static_assert( alignof( TileLayer ) == 8, "Double-check TileLayer alignment" );

///////////////////////////////////////////////////////////////////////////////
}}
