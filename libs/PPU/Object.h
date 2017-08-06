#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core/macros.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Object
{
	RF_NO_COPY(Object);

	//
	// Public methods
public:
	Object() = default;
	Object( Object && ) = default;
	~Object() = default;
	Object& operator =( Object && ) = default;


	//
	// Public data
public:
	uint16_t m_FramePackID;

	uint8_t m_XCoord;
	uint8_t m_YCoord;
	uint8_t m_ZLayer;

	uint8_t m_GFXModifier;
	uint8_t m_TimeIndex;
	uint8_t m_TimeSlowdown;

	bool m_Looping : 1;
	bool m_HorizontalFlip : 1;
	bool m_VerticalFlip : 1;
	bool m_UseTransform : 1;
	int reserved : 12;

	float m_Transform[2][2];
};

///////////////////////////////////////////////////////////////////////////////
}}
