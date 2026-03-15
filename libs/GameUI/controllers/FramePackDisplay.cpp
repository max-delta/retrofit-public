#include "stdafx.h"
#include "FramePackDisplay.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

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
	mFramePack.SetFramePack( framePack, maxTimeIndex, expectedWidth, expectedHeight );
	mFramePack.ResetAnimationTimer();
}



void FramePackDisplay::SetSlowdown( gfx::TimeSlowdownRate rate )
{
	mFramePack.SetSlowdown( rate );
}



void FramePackDisplay::SetJustification( Justification::Value justification )
{
	mFramePack.SetJustification( justification );
}



void FramePackDisplay::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::ppu::DepthLayer const zLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mFramePack.Render( renderer, container.mAABB, zLayer );
}

///////////////////////////////////////////////////////////////////////////////
}
