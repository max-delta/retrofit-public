#pragma once
#include "project.h"

#include "PPU/PPUTimer.h"
#include "PPU/Tileset.h"

#include "core/macros.h"

#include "rftl/vector"


namespace RF::gfx::ppu {
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
	using TileZoomFactor = uint8_t;
	static constexpr TileZoomFactor kTileZoomFactor_Quarter = 1; // 2^(N-3)=1/4
	static constexpr TileZoomFactor kTileZoomFactor_Half = 2; // 2^(N-3)=1/2
	static constexpr TileZoomFactor kTileZoomFactor_Normal = 3; // 2^(N-3)=1
	static constexpr TileZoomFactor kTileZoomFactor_Double = 4; // 2^(N-3)=2
	static constexpr TileZoomFactor kTileZoomFactor_Quadruple = 5; // 2^(N-3)=4


	//
	// Structs
public:
	struct Tile
	{
		TileIndex mIndex = kEmptyTileIndex;
	};


	//
	// Public methods
public:
	TileLayer() = default;

	void Animate();

	size_t NumColumns() const;
	size_t NumRows() const;
	size_t NumTiles() const;

	Tile const& GetTile( size_t column, size_t row ) const;
	Tile const& GetTile( size_t index ) const;
	Tile& GetMutableTile( size_t column, size_t row );
	Tile& GetMutableTile( size_t index );

	void GetTileZoomFactor( uint8_t& numer, uint8_t& denom ) const;

	Tile const* Data() const;
	Tile* MutableData();

	void Clear();
	void ClearAndResize( size_t columns, size_t rows );


	//
	// Public data
public:
	ManagedTilesetID mTilesetReference;
	uint16_t mNumColumns;

	CoordElem mXCoord;
	CoordElem mYCoord;
	DepthLayer mZLayer;

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

	TileZoomFactor mTileZoomFactor;

	float mTransform[2][2];


	//
	// Private data
private:
	rftl::vector<Tile> mTiles;
};
static_assert( sizeof( TileLayer ) <= 96, "Double-check TileLayer storage" );
static_assert( alignof( TileLayer ) == RF::compiler::kPointerBytes, "Double-check TileLayer alignment" );

///////////////////////////////////////////////////////////////////////////////
}
