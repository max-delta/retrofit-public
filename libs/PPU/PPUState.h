#pragma once
#include "project.h"

#include "PPU/PPUConfig.h"
#include "PPU/Object.h"
#include "PPU/TileLayer.h"

#include "core_math/Color3f.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUState
{
	RF_NO_COPY( PPUState );
	RF_NO_MOVE( PPUState );

	// Constants
public:
	static constexpr size_t kMaxObjects = ppu::kMaxObjects;
	static constexpr size_t kMaxTileLayers = ppu::kMaxTileLayers;
	static constexpr size_t kMaxStrings = ppu::kMaxStrings;
	static constexpr size_t kMaxTotalElements =
		kMaxObjects +
		kMaxTileLayers +
		kMaxStrings;


	//
	// Structs
public:
	struct String
	{
		static constexpr size_t k_MaxLen = kMaxStringLen;

		PPUCoordElem mXCoord;
		PPUCoordElem mYCoord;
		PPUDepthLayer mZLayer;
		uint8_t mColor[3];
		uint8_t mDesiredHeight;
		bool mBorder : 1;
		bool mReserved10 : 7;
		ManagedFontID mFontReference;
		char mText[k_MaxLen + sizeof( '\0' )];
	};
	static_assert( sizeof( String ) == 84, "Double-check String storage" );
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

	Object mObjects[kMaxObjects];
	TileLayer mTileLayers[kMaxTileLayers];
	String mStrings[kMaxStrings];
};
// NOTE: If you've modified the PPU to have higher limits, you'll want to
//  re-tune these as well, or disable them entirely if you don't care.
static_assert( offsetof( PPUState, mObjects ) <= 32, "Double-check PPUState storage" );
static_assert( sizeof( PPUState::mObjects ) == 1024, "Double-check PPUState storage" );
static_assert( sizeof( PPUState::mTileLayers ) <= 2048, "Double-check PPUState storage" );
static_assert( offsetof( PPUState, mStrings ) <= 4096, "Double-check PPUState storage" );
static_assert( sizeof( PPUState::mStrings ) <= 4096, "Double-check PPUState storage" );
static_assert( sizeof( PPUState ) <= 8192, "Double-check PPUState storage" );
static_assert( alignof( PPUState ) == RF::compiler::kPointerBytes, "Double-check PPUState alignment" );

///////////////////////////////////////////////////////////////////////////////
}
