#pragma once
#include "project.h"

#include "PPUState.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUDebugState
{
	RF_NO_COPY( PPUDebugState );
	RF_NO_MOVE( PPUDebugState );

	// Constants
public:
	static constexpr size_t kMaxDebugStrings = ppu::kMaxDebugStrings;
	static constexpr size_t kMaxDebugAuxStrings = ppu::kMaxDebugAuxStrings;
	static constexpr size_t kMaxDebugLines = ppu::kMaxDebugLines;
	static constexpr size_t kMaxTotalElements =
		kMaxDebugStrings +
		kMaxDebugAuxStrings +
		kMaxDebugLines;


	//
	// Structs
public:
	struct DebugString
	{
		static constexpr size_t k_MaxLen = kMaxDebugStringLen;

		CoordElem mXCoord;
		CoordElem mYCoord;
		rftl::array<char, k_MaxLen + sizeof( '\0' )> mText;
	};
	struct DebugAuxString : PPUState::String
	{
		//
	};
	struct DebugLine
	{
		CoordElem mXCoord0;
		CoordElem mYCoord0;
		CoordElem mXCoord1;
		CoordElem mYCoord1;
		CoordElem mWidth;
		DepthLayer mZLayer;
		math::Color3u8 mColor;
	};


	//
	// Public methods
public:
	PPUDebugState() = default;
	~PPUDebugState() = default;

	void Clear();


	//
	// Public data
public:
	uint8_t mNumStrings;
	uint8_t mNumAuxStrings;
	uint8_t mNumLines;

	rftl::array<DebugString, kMaxDebugStrings> mStrings;
	rftl::array<DebugAuxString, kMaxDebugAuxStrings> mAuxStrings;
	rftl::array<DebugLine, kMaxDebugLines> mLines;
	TextStorage<kMaxDebugTextStorage> mTextStorage;
};

///////////////////////////////////////////////////////////////////////////////
}
