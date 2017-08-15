#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core/macros.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Object
{
	//
	// Public methods
public:
	Object() = default;
	~Object() = default;

	void Animate();


	//
	// Public data
public:
	static_assert( sizeof( ManagedFramePackID ) == sizeof( uint16_t ), "Re-evaluate packing" );
	ManagedFramePackID m_FramePackID;

	static_assert( sizeof( PPUCoordElem ) == sizeof( int16_t ), "Re-evaluate packing" );
	PPUCoordElem m_XCoord;
	PPUCoordElem m_YCoord;
	static_assert( sizeof( PPUDepthLayer ) == sizeof( int8_t ), "Re-evaluate packing" );
	PPUDepthLayer m_ZLayer;

	uint8_t m_GFXModifier;
	uint8_t m_TimeIndex;
	TimeSlowdownRate m_TimeSlowdown; // 1=60, 2=30, 3=20, 4=15... MAX=pause
	uint8_t m_SubTimeIndex;

	bool m_Paused : 1;
	bool m_HorizontalFlip : 1;
	bool m_VerticalFlip : 1;
	bool m_UseTransform : 1;
	uint8_t reserved1 : 4;
	uint8_t reserved2 : 8;

	float m_Transform[2][2];
};
static_assert( sizeof( Object ) == 32, "Double-check Object storage" );
static_assert( alignof( Object ) == 4, "Double-check Object alignment" );

///////////////////////////////////////////////////////////////////////////////
}}
