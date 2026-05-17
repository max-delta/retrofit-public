#include "stdafx.h"
#include "Object.h"

#include "PPU/FramePackRef.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

void Object::SetFromFramePackRef( const gfx::ppu::FramePackRef& framePack )
{
	static_assert(
		sizeof( FramePackRef ) == 4,
		"Did FramePackRef add more members that are relevant here?" );
	mFramePackID = framePack.mManagedID;
	mTimer.mMaxTimeIndex = framePack.mMaxTimeIndex;
	mTimer.mTimeSlowdown = framePack.mPreferredSlowdownRate;
}



void Object::Animate()
{
	mTimer.Animate( mLooping, mPaused );
}



void Object::ResetAnimation()
{
	mTimer.ResetAnimation();
}

///////////////////////////////////////////////////////////////////////////////
}
