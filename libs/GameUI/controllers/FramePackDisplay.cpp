#include "stdafx.h"
#include "FramePackDisplay.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::FramePackDisplay )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::FramePackDisplay );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

void FramePackDisplay::SetFramePack( gfx::ppu::ManagedFramePackID framePack, uint8_t maxTimeIndex, gfx::ppu::CoordElem expectedWidth, gfx::ppu::CoordElem expectedHeight )
{
	mObject.mFramePackID = framePack;
	mObject.mTimer.mMaxTimeIndex = maxTimeIndex;
	mExpectedDimensions = { expectedWidth, expectedHeight };
}



void FramePackDisplay::SetSlowdown( gfx::TimeSlowdownRate rate )
{
	mObject.mTimer.mTimeSlowdown = rate;
}



void FramePackDisplay::SetJustification( Justification::Value justification )
{
	mJustification = justification;
}



void FramePackDisplay::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::ppu::Coord const pos = AlignToJustify( mExpectedDimensions, container.mAABB, mJustification );

	mObject.mXCoord = pos.x;
	mObject.mYCoord = pos.y;
	mObject.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mObject.mLooping = true;
	mObject.Animate();

	renderer.DrawObject( mObject );
}

///////////////////////////////////////////////////////////////////////////////
}
