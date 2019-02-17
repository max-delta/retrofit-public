#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/Object.h"
#include "PPU/TileLayer.h"

#include "core_math/Color3f.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUState
{
	RF_NO_COPY( PPUState );
	RF_NO_MOVE( PPUState );

	// Constants
public:
	static constexpr size_t kMaxObjects = 32;
	static constexpr size_t kMaxTileLayers = 16;
	static constexpr size_t kMaxStrings = 30;
	static constexpr size_t kMaxTotalElements =
		kMaxObjects +
		kMaxTileLayers +
		kMaxStrings;


	//
	// Structs
public:
	struct String
	{
		static constexpr size_t k_MaxLen = 123;

		PPUCoordElem mXCoord;
		PPUCoordElem mYCoord;
		PPUDepthLayer mZLayer;
		uint8_t mColor[3];
		uint8_t mDesiredHeight;
		ManagedFontID mFontReference;
		char mText[k_MaxLen + sizeof( '\0' )];
	};
	static_assert( sizeof( String ) == 136, "Double-check String storage" );
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
}}
