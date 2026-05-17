#include "stdafx.h"
#include "FramePackHelper.h"

#include "GameUI/AlignmentHelpers.h"

#include "PPU/PPUController.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

void FramePackHelper::SetFramePack( const gfx::ppu::FramePackRef& framePack, gfx::ppu::CoordElem expectedWidth, gfx::ppu::CoordElem expectedHeight )
{
	mObject.SetFromFramePackRef( framePack );
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



void FramePackHelper::SetHorizontalFlip( bool flipped )
{
	mObject.mHorizontalFlip = flipped;
}



void FramePackHelper::SetVerticalFlip( bool flipped )
{
	mObject.mVerticalFlip = flipped;
}



void FramePackHelper::ResetAnimationTimer()
{
	mObject.ResetAnimation();
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
