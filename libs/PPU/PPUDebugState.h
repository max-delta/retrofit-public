#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core_math/Vector2.h"
#include "core_math/Color3f.h"

#include "core/macros.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUDebugState
{
	RF_NO_COPY( PPUDebugState );
	RF_NO_MOVE( PPUDebugState );

	// Constants
public:
	static constexpr size_t kMaxStrings = 32;
	static constexpr size_t kMaxLines = 32;
	static constexpr size_t kMaxTotalElements =
		kMaxStrings +
		kMaxLines;


	//
	// Structs
public:
	struct DebugString
	{
		static constexpr size_t k_MaxLen = 127;

		PPUCoordElem mXCoord;
		PPUCoordElem mYCoord;
		char mText[k_MaxLen + sizeof( '\0' )];
	};
	struct DebugLine
	{
		PPUCoordElem m_XCoord0;
		PPUCoordElem m_YCoord0;
		PPUCoordElem m_XCoord1;
		PPUCoordElem m_YCoord1;
		PPUCoordElem mWidth;
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
	uint8_t mNumLines;
	DebugString mStrings[kMaxStrings];
	DebugLine mLines[kMaxLines];
};

///////////////////////////////////////////////////////////////////////////////
}}
