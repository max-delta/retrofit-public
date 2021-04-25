#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

#include "PPU/PPUTimer.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Non-binding, output-only
class OverworldVisual final : public state::NonBindingComponent
{
	RF_NO_COPY( OverworldVisual );

	//
	// Structs
public:
	struct Anim
	{
		gfx::ppu::ManagedFramePackID mFramePackID = gfx::ppu::kInvalidManagedFramePackID;
		gfx::TimeSlowdownRate mSlowdownRate = gfx::kTimeSlowdownRate_Normal;
		uint8_t mMaxTimeIndex = 0;
	};


	//
	// Public methods
public:
	OverworldVisual() = default;

	//
	// Public data
public:
	gfx::PPUTimer mTimer = {};

	Anim mWalkNorth = {};
	Anim mWalkEast = {};
	Anim mWalkSouth = {};
	Anim mWalkWest = {};

	Anim mIdleNorth = {};
	Anim mIdleEast = {};
	Anim mIdleSouth = {};
	Anim mIdleWest = {};
};

///////////////////////////////////////////////////////////////////////////////
}
