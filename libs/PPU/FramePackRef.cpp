#include "stdafx.h"
#include "FramePackRef.h"

#include "PPU/FramePack.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

FramePackRef FramePackRef::FromFramePack( ManagedFramePackID id, FramePackBase const& contents )
{
	return FramePackRef{
		.mManagedID = id,
		.mMaxTimeIndex = contents.CalculateTimeIndexBoundary(),
		.mPreferredSlowdownRate = contents.mPreferredSlowdownRate,
	};
}

///////////////////////////////////////////////////////////////////////////////
}
