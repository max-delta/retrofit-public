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

ElementSlotOverlay::ElementSlotOverlay( ElementTileSize size )
	: mSize( size )
{
	//
}



gfx::ppu::Coord ElementSlotOverlay::CalcContainerDimensions( ElementTileSize size )
{
	switch( size )
	{
		case ElementTileSize::Full:
			return {
				kElementTilesetFull.mTileWidth,
				kElementTilesetFull.mTileHeight };
		case ElementTileSize::Medium:
			return {
				kElementTilesetMedium.mTileWidth,
				kElementTilesetMedium.mTileHeight };
		case ElementTileSize::Mini:
		case ElementTileSize::Micro:
		case ElementTileSize::Invalid:
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



void ElementSlotOverlay::UpdateState( bool active, bool selected )
{
	if( active == false )
	{
		// Inactive
		mNameLabel->SetColor( math::Color3f::kGray50 );
	}
	else if( selected )
	{
		// Selected
		mNameLabel->SetColor( math::Color3f::kYellow );
	}
	else
	{
		// Unselected
		mNameLabel->SetColor( math::Color3f::kWhite );
	}
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
		case ElementTileSize::Full:
			tilesetDef = kElementTilesetFull;
			break;
		case ElementTileSize::Medium:
			tilesetDef = kElementTilesetMedium;
			break;
		case ElementTileSize::Mini:
		case ElementTileSize::Micro:
		case ElementTileSize::Invalid:
		default:
			RF_DBGFAIL();
	}
	RF_ASSERT( tilesetDef.mFont != kInvalidFontPurposeID );

	// Name
	mNameLabel =
		uiManager.AssignStrongController(
			nameContainerID,
			DefaultCreator<TextLabel>::Create() );
	mNameLabel->SetJustification( ui::Justification::MiddleCenter );
	mNameLabel->SetFont( tilesetDef.mFont );
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
