#pragma once
#include "project.h"

#include "PPU/PPUTimer.h"

#include "core/macros.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Object
{
	//
	// Public methods
public:
	Object() = default;

	void Animate();


	//
	// Public data
public:
	static_assert( sizeof( ManagedFramePackID ) == sizeof( uint16_t ), "Re-evaluate packing" );
	ManagedFramePackID mFramePackID = kInvalidManagedFramePackID;

	static_assert( sizeof( CoordElem ) == sizeof( int16_t ), "Re-evaluate packing" );
	CoordElem mXCoord = 0;
	CoordElem mYCoord = 0;
	static_assert( sizeof( DepthLayer ) == sizeof( int8_t ), "Re-evaluate packing" );
	DepthLayer mZLayer = 0;

	uint8_t mGFXModifier = 0;
	PPUTimer mTimer = {};

	bool mLooping : 1;
	bool mPaused : 1;
	bool mHorizontalFlip : 1;
	bool mVerticalFlip : 1;
	bool mUseTransform : 1;
	uint8_t reserved1 : 3;
	uint8_t reserved2 : 8;
	uint8_t reserved3 : 8;
	uint8_t reserved4 : 8;

	float mTransform[2][2] = {};
};
static_assert( sizeof( Object ) == 32, "Double-check Object storage" );
static_assert( alignof( Object ) == 4, "Double-check Object alignment" );

///////////////////////////////////////////////////////////////////////////////
}
