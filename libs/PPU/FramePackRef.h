#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

// Frame packs only have a few fields necessary for actually getting them set up
//  on an animating object, so these can be passed around without needing to
//  have access to the full frame pack
class PPU_API FramePackRef
{
	//
	// Public methods
public:
	static FramePackRef FromFramePack( ManagedFramePackID id, const FramePackBase& contents );


	//
	// Public data
public:
	gfx::ppu::ManagedFramePackID mManagedID = gfx::ppu::kInvalidManagedFramePackID;
	uint8_t mMaxTimeIndex = 0;
	gfx::TimeSlowdownRate mPreferredSlowdownRate = 0;
};
static_assert( alignof( FramePackRef ) == 2, "Double-check FramePackRef alignment" );
static_assert( sizeof( FramePackRef ) == 4, "Double-check FramePackRef storage" );

///////////////////////////////////////////////////////////////////////////////
}
