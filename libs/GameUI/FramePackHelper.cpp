#include "stdafx.h"
#include "FramePackHelper.h"

#include "GameUI/AlignmentHelpers.h"

#include "PPU/PPUController.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

void FramePackHelper::SetFramePack( gfx::ppu::ManagedFramePackID framePack, uint8_t maxTimeIndex, gfx::ppu::CoordElem expectedWidth, gfx::ppu::CoordElem expectedHeight )
{
	mObject.mFramePackID = framePack;
	mObject.mTimer.mMaxTimeIndex = maxTimeIndex;
	mExpectedDimensions = { expectedWidth, expectedHeight };
}



void FramePackHelper::SetSlowdown( gfx::TimeSlowdownRate rate )
{
	mObject.mTimer.mTimeSlowdown = rate;
}



void FramePackHelper::SetJustification( Justification::Value justification )
{
	mJustification = justification;
}



void FramePackHelper::Render( gfx::ppu::PPUController& renderer, gfx::ppu::AABB const& bounds, gfx::ppu::DepthLayer zLayer )
{
	gfx::ppu::Coord const pos = AlignToJustify( mExpectedDimensions, bounds, mJustification );

	mObject.mXCoord = pos.x;
	mObject.mYCoord = pos.y;
	mObject.mZLayer = zLayer;
	mObject.mLooping = true;
	mObject.Animate();

	renderer.DrawObject( mObject );
}

///////////////////////////////////////////////////////////////////////////////
}
