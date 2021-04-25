#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

struct PPU_API PPUTimer
{
	//
	// Public methods
public:
	void Animate( bool looping, bool paused );
	bool IsFullZero() const;


	//
	// Public data
public:
	uint8_t mTimeIndex;
	uint8_t mMaxTimeIndex;
	TimeSlowdownRate mTimeSlowdown; // 1=60, 2=30, 3=20, 4=15... MAX=pause
	uint8_t mSubTimeIndex;
};
static_assert( sizeof( PPUTimer ) == 4, "Double-check PPUTimer storage" );
static_assert( alignof( PPUTimer ) == 1, "Double-check PPUTimer alignment" );

///////////////////////////////////////////////////////////////////////////////
}
