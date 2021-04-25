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

		PPUCoordElem mXCoord;
		PPUCoordElem mYCoord;
		char mText[k_MaxLen + sizeof( '\0' )];
	};
	struct DebugAuxString : PPUState::String
	{
		//
	};
	struct DebugLine
	{
		PPUCoordElem mXCoord0;
		PPUCoordElem mYCoord0;
		PPUCoordElem mXCoord1;
		PPUCoordElem mYCoord1;
		PPUCoordElem mWidth;
		PPUDepthLayer mZLayer;
		math::Color3f mColor;
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
	DebugString mStrings[kMaxDebugStrings];
	DebugAuxString mAuxStrings[kMaxDebugAuxStrings];
	DebugLine mLines[kMaxDebugLines];
};

///////////////////////////////////////////////////////////////////////////////
}
