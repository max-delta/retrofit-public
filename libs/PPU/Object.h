#pragma once
#include "project.h"

#include "PPU/PPUTimer.h"

#include "core/macros.h"


namespace RF { namespace gfx {
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
	ManagedFramePackID mFramePackID;

	static_assert( sizeof( PPUCoordElem ) == sizeof( int16_t ), "Re-evaluate packing" );
	PPUCoordElem mXCoord;
	PPUCoordElem mYCoord;
	static_assert( sizeof( PPUDepthLayer ) == sizeof( int8_t ), "Re-evaluate packing" );
	PPUDepthLayer mZLayer;

	uint8_t mGFXModifier;
	PPUTimer mTimer;

	bool mLooping : 1;
	bool mPaused : 1;
	bool mHorizontalFlip : 1;
	bool mVerticalFlip : 1;
	bool mUseTransform : 1;
	uint8_t reserved1 : 3;
	uint8_t reserved2 : 8;
	uint8_t reserved3 : 8;
	uint8_t reserved4 : 8;

	float mTransform[2][2];
};
static_assert( sizeof( Object ) == 32, "Double-check Object storage" );
static_assert( alignof( Object ) == 4, "Double-check Object alignment" );

///////////////////////////////////////////////////////////////////////////////
}}
