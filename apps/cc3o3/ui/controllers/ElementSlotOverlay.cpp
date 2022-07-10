#include "stdafx.h"
#include "ElementSlotOverlay.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
//#include "GameUI/FocusEvent.h"
#include "GameUI/controllers/TextLabel.h"

#include "RFType/CreateClassInfoDefinition.h"

//#include "PPU/PPUController.h"
//#include "PPU/TilesetManager.h"

//#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::ElementSlotOverlay )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::ElementSlotOverlay );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

ElementSlotOverlay::ElementSlotOverlay( Size size )
	: mSize( size )
{
	//
}



gfx::ppu::Coord ElementSlotOverlay::CalcContainerDimensions( Size size )
{
	switch( size )
	{
		case Size::Full:
			return {
				kElementTilesetFull.mTileWidth,
				kElementTilesetFull.mTileHeight };
		default:
			RF_DBGFAIL();
			return {};
	}
}



void ElementSlotOverlay::UpdateFromCache( ElementSlotDisplayCache const& cache )
{
	mCache = cache;
	UpdateDisplay();
}



void ElementSlotOverlay::OnInstanceAssign( UIContext& context, Container& container )
{
	ui::ContainerManager& uiManager = context.GetMutableContainerManager();

	ContainerID const nameContainerID = CreateChildContainer(
		uiManager,
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	ElementTilesetDef tilesetDef = {};
	switch( mSize )
	{
		case Size::Full:
			tilesetDef = kElementTilesetFull;
			break;
		default:
			RF_DBGFAIL();
	}
	RF_ASSERT( tilesetDef.mSupportsText );

	// Name
	mNameLabel =
		uiManager.AssignStrongController(
			nameContainerID,
			DefaultCreator<TextLabel>::Create() );
	mNameLabel->SetJustification( ui::Justification::MiddleCenter );
	mNameLabel->SetFont( ui::font::LargeMenuText );
	mNameLabel->SetText( "UNSET" );
	mNameLabel->SetColor( math::Color3f::kWhite );
	mNameLabel->SetBorder( true );
}



void ElementSlotOverlay::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	gfx::ppu::Coord const expectedDimensions = CalcContainerDimensions( mSize );
	RF_ASSERT_MSG( container.mAABB.Width() == expectedDimensions.x, "Container not sized as needed" );
	RF_ASSERT_MSG( container.mAABB.Height() == expectedDimensions.y, "Container not sized as needed" );
}

///////////////////////////////////////////////////////////////////////////////

void ElementSlotOverlay::UpdateDisplay()
{
	// Name
	mNameLabel->SetText( mCache.mName );
}

///////////////////////////////////////////////////////////////////////////////
}
