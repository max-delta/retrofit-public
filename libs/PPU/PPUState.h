#pragma once
#include "project.h"

#include "PPU/PPUConfig.h"
#include "PPU/Object.h"
#include "PPU/TileLayer.h"
#include "PPU/TextStorage.h"

#include "core_math/Color3u8.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUState
{
	RF_NO_COPY( PPUState );
	RF_NO_MOVE( PPUState );

	//
	// Constants
public:
	static constexpr size_t kMaxObjects = ppu::kMaxObjects;
	static constexpr size_t kMaxTileLayers = ppu::kMaxTileLayers;
	static constexpr size_t kMaxStrings = ppu::kMaxStrings;
	static constexpr size_t kMaxTotalElements =
		kMaxObjects +
		kMaxTileLayers +
		kMaxStrings;
	static constexpr size_t kMaxTextStorage = ppu::kMaxTextStorage;


	//
	// Structs
public:
	struct String
	{
		CoordElem mXCoord;
		CoordElem mYCoord;
		DepthLayer mZLayer;
		math::Color3u8 mColor;
		uint8_t mDesiredHeight;
		bool mBorder : 1;
		bool mReserved10 : 7;
		ManagedFontID mFontReference;
		uint16_t mTextOffset;
	};
	static_assert( sizeof( String ) == 14, "Double-check String storage" );
	static_assert( alignof( String ) == 2, "Double-check String alignment" );


	//
	// Public methods
public:
	PPUState() = default;
	~PPUState() = default;

	void Clear();


	//
	// Public data
public:
	uint8_t mNumObjects;
	uint8_t mNumTileLayers;
	uint8_t mNumStrings;

	rftl::array<Object, kMaxObjects> mObjects;
	rftl::array<TileLayer, kMaxTileLayers> mTileLayers;
	rftl::array<String, kMaxStrings> mStrings;
	TextStorage<kMaxTextStorage> mTextStorage;
};
// NOTE: If you've modified the PPU to have higher limits, you'll want to
//  re-tune these as well, or disable them entirely if you don't care.
static_assert( sizeof( rftl::array<int, 3> ) == sizeof( int[3] ), "Expected arrays to have no overhead" );
RF_CLANG_PUSH();
RF_CLANG_IGNORE( "-Winvalid-offsetof" ); // Technically incorrect, fine in practice
static_assert( offsetof( PPUState, mObjects ) <= 32, "Double-check PPUState storage" );
static_assert( sizeof( PPUState::mObjects ) == 1024, "Double-check PPUState storage" );
static_assert( sizeof( PPUState::mTileLayers ) <= 2048, "Double-check PPUState storage" );
static_assert( offsetof( PPUState, mStrings ) <= 2184, "Double-check PPUState storage" );
static_assert( sizeof( PPUState::mStrings ) <= 672, "Double-check PPUState storage" );
static_assert( offsetof( PPUState, mTextStorage ) <= 2856, "Double-check PPUState storage" );
static_assert( sizeof( PPUState::mTextStorage ) <= 1240, "Double-check PPUState storage" );
static_assert( sizeof( PPUState ) <= 4096, "Double-check PPUState storage" );
static_assert( alignof( PPUState ) == RF::compiler::kPointerBytes, "Double-check PPUState alignment" );
RF_CLANG_POP();

///////////////////////////////////////////////////////////////////////////////
}
