#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core/macros.h"

#include "rftl/vector"


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

	struct TileAnim
	{
		uint8_t mReserved : 2 = 0;

		// Whether the 'begin' of the animation is considered to be shifted
		//  forward, intended to allow tricks like having the same animation at
		//  multiple staggered offsets on a tile layer, without actually using
		//  up any more tiles to cause that offset
		// NOTE: The offset is positive, and will be subtracted from the start
		//  frame to determine where the new 'begin' is when the animation
		//  needs to loop over
		// NOTE: The offset cannot exceed the maximum number of frames
		static constexpr uint8_t kMaxOffsetFrames = kTileAnimFrameMax;
		static_assert( kTileAnimFrameMax == 0b110 );
		uint8_t mOffsetFrames : 3 = 0;

		// How many frames of animation are in the animation
		// NOTE: Zero frames is invalid, meaning this tile does not animate
		static constexpr uint8_t kMaxAnimFrames = kTileAnimFrameMax;
		static_assert( kTileAnimFrameMax == 0b110 );
		uint8_t mNumAnimFrames : 3 = 0;

		// The count for frames technically can hold more bits (0b111), but
		//  we're intentionally capping at 6 (0b110), because we want to be
		//  able to do clean timer logic, so 7 (0b111) is reserved for future
		//  use as a special value
		// SEE: kTileAnimFrameMax comments
		static constexpr uint8_t kReservedFrameCountValue = 0b111;
	};
	static_assert( TileAnim::kMaxAnimFrames == TileAnim::kMaxOffsetFrames );
	static_assert( sizeof( TileAnim ) == 1, "Double-check TileAnim storage" );
	static_assert( alignof( TileAnim ) == 1, "Double-check TileAnim alignment" );

	// Mapping is direct tile index -> anim data, with anything past the vector
	//  size being invalid, and anything with 0 frames of animation also being
	//  invalid, allowing this to act like a map but be implemented as a vector
	using TileAnims = rftl::vector<TileAnim>;


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
	// Private methods
private:
	TileAnim GetTileAnim( TileIndex index ) const;
	static TileAnim& GetMutableTileAnim( TileAnims& tileAnims, TileIndex index );


	//
	// Private data
private:
	ManagedTextureID mTextureReference = kInvalidManagedTextureID;
	TileAnims mTileAnims = {};
	TileSize mTileWidth = kDefaultTileSize;
	TileSize mTileHeight = kDefaultTileSize;
};
static_assert( sizeof( Tileset ) <= 48, "Double-check Tileset storage" );
static_assert( alignof( Tileset ) == 8, "Double-check Tileset alignment" );

///////////////////////////////////////////////////////////////////////////////
}
