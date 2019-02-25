#include "stdafx.h"
#include "FramePackDisplay.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/Lerp.h"


RFTYPE_CREATE_META( RF::ui::controller::FramePackDisplay )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::FramePackDisplay );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

void FramePackDisplay::SetFramePack( gfx::ManagedFramePackID framePack, gfx::PPUCoordElem expectedWidth, gfx::PPUCoordElem expectedHeight )
{
	mObject.mFramePackID = framePack;
	mExpectedDimensions = { expectedWidth, expectedHeight };
}



void FramePackDisplay::SetJustification( Justification justification )
{
	mJustification = justification;
}



void FramePackDisplay::OnRender( ContainerManager const& manager, Container const& container, bool& blockChildRendering )
{
	gfx::PPUController& renderer = GetRenderer( manager );

	gfx::PPUCoord pos = container.mAABB.mTopLeft;
	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Top ) )
	{
		pos.y = container.mAABB.Top();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Bottom ) )
	{
		pos.y = container.mAABB.Bottom() - mExpectedDimensions.y;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Middle ) )
	{
		gfx::PPUCoordElem const top = container.mAABB.Top();
		gfx::PPUCoordElem const bottom = container.mAABB.Bottom() - mExpectedDimensions.y;
		pos.y = math::Lerp( top, bottom, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Left ) )
	{
		pos.x = container.mAABB.Left();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Right ) )
	{
		pos.x = container.mAABB.Right() - mExpectedDimensions.x;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Center ) )
	{
		gfx::PPUCoordElem const left = container.mAABB.Left();
		gfx::PPUCoordElem const right = container.mAABB.Right() - mExpectedDimensions.x;
		pos.x = math::Lerp( left, right, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	mObject.mXCoord = pos.x;
	mObject.mYCoord = pos.y;
	mObject.mZLayer = manager.GetRecommendedRenderDepth( container );
	mObject.mLooping = true;
	mObject.Animate();

	renderer.DrawObject( mObject );
}

///////////////////////////////////////////////////////////////////////////////
}}}
