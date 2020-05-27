#include "stdafx.h"
#include "CharacterSlot.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/SiteVisual.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/FramePackDisplay.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::CharacterSlot )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::CharacterSlot );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

void CharacterSlot::ClearCharacter()
{
	mBorderFrame->SetChildRenderingBlocked( true );
}



void CharacterSlot::UpdateCharacter( state::ObjectRef const& character )
{
	RF_ASSERT( character.IsSet() );
	mBorderFrame->SetChildRenderingBlocked( false );

	state::comp::SiteVisual const& visual = *character.GetComponentInstanceT<state::comp::SiteVisual>();
	state::comp::SiteVisual::Anim const& anim = visual.mIdleSouth;
	mCharacterDisplay->SetFramePack(
		anim.mFramePackID,
		anim.mMaxTimeIndex,
		( gfx::kTileSize / 4 ) * 3,
		( gfx::kTileSize / 4 ) * 5 );

	// TODO: Update text
}



void CharacterSlot::OnInstanceAssign( UIContext& context, Container& container )
{
	ui::ContainerManager& uiManager = context.GetMutableContainerManager();
	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();

	mChildContainerID = CreateChildContainer(
		uiManager,
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	// Frame
	mBorderFrame =
		uiManager.AssignStrongController(
			mChildContainerID,
			DefaultCreator<BorderFrame>::Create() );
	mBorderFrame->SetTileset( context, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );
	mBorderFrame->SetChildRenderingBlocked( true );

	// 2 subsections for info and display
	ColumnSlicer::Ratios const charSlotColumnRatios = {
		{ 7.f / 10.f, true },
		{ 3.f / 10.f, true },
	};
	WeakPtr<ColumnSlicer> const charSlotColumnSlicer =
		uiManager.AssignStrongController(
			mBorderFrame->GetChildContainerID(),
			DefaultCreator<ColumnSlicer>::Create(
				charSlotColumnRatios ) );

	// 3 info rows
	RowSlicer::Ratios const infoRowRatios = {
		{ 1.f / 3.f, true },
		{ 1.f / 3.f, true },
		{ 1.f / 3.f, true },
	};
	WeakPtr<RowSlicer> const infoRowSlicer =
		uiManager.AssignStrongController(
			charSlotColumnSlicer->GetChildContainerID( 0 ),
			DefaultCreator<RowSlicer>::Create(
				infoRowRatios ) );

	for( size_t i_info = 0; i_info < 3; i_info++ )
	{
		// Info
		WeakPtr<TextLabel>& info = mInfoRows.at( i_info );
		info =
			uiManager.AssignStrongController(
				infoRowSlicer->GetChildContainerID( i_info ),
				DefaultCreator<TextLabel>::Create() );
		info->SetJustification( ui::Justification::MiddleLeft );
		info->SetFont( ui::font::LargeMenuText );
		info->SetText( "UNSET" );
		info->SetColor( math::Color3f::kWhite );
		info->SetBorder( true );
	}

	// Display
	mCharacterDisplay =
		uiManager.AssignStrongController(
			charSlotColumnSlicer->GetChildContainerID( 1 ),
			DefaultCreator<FramePackDisplay>::Create() );
	mCharacterDisplay->SetJustification( ui::Justification::MiddleCenter );
}

///////////////////////////////////////////////////////////////////////////////
}
