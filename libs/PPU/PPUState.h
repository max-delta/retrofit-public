#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/Object.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUState
{
	RF_NO_COPY( PPUState );
	RF_NO_MOVE( PPUState );

	// Constants
public:
	static constexpr size_t kMaxObjects = 32;
	static constexpr size_t kMaxStrings = 32;


	//
	// Structs
public:
	struct String
	{
		static constexpr size_t k_MaxLen = 127;

		PPUCoordElem mXCoord;
		PPUCoordElem mYCoord;
		PPUCoordElem mWidth;
		PPUCoordElem mHeight;
		char mText[k_MaxLen + sizeof( '\0' )];
	};


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
	uint8_t mNumStrings;
	Object mObjects[kMaxObjects];
	String mStrings[kMaxStrings];
};

///////////////////////////////////////////////////////////////////////////////
}}
