#pragma once
#include "project.h"

#include "cc3o3/state/Component.h"

#include "PPU/Object.h"

#include "core/macros.h"


namespace RF::cc::state::comp {
///////////////////////////////////////////////////////////////////////////////

class OverworldVisual : public state::Component
{
	RF_NO_COPY( OverworldVisual );

	//
	// Structs
public:
	struct Anim
	{
		gfx::ManagedFramePackID mFramePackID = gfx::kInvalidManagedFramePackID;
		uint8_t mMaxTimeIndex = 0;
	};


	//
	// Public methods
public:
	OverworldVisual() = default;


	//
	// Public data
public:
	gfx::Object mObject = {};

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
