#include "stdafx.h"
#include "TitleScreen_CharCreate.h"

#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/MessageBox.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/TextRows.h"
#include "GameUI/controllers/ListBox.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/FramePackManager.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr char kReturnTag[] = "RETURN";

}
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_CharCreate::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_CharCreate::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Cut the whole screen into rows
		ui::controller::RowSlicer::Ratios const rootRowRatios = {
			{ 5.f / 7.f, true },
			{ 2.f / 7.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const rootRowSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::RowSlicer>::Create(
					rootRowRatios ) );

		// Put a message box at the bottom
		WeakPtr<ui::controller::MessageBox> const messageBox =
			uiManager.AssignStrongController(
				rootRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::MessageBox>::Create(
					3u,
					ui::font::MessageBox,
					ui::Justification::MiddleLeft,
					math::Color3f::kWhite,
					ui::GetBreakableChars() ) );
		messageBox->SetFrameTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), { 4, 4 }, { 2, 0 } );
		messageBox->SetAnimationSpeed( ui::kTextSpeed );
		messageBox->SetText(
			"This is some text. It may take up a fair amount of space, so it"
			" will likely get broken up by the text box. It may even get"
			" truncated due to its large size... Like right about here... Or"
			" maybe here? If you're using widescreen, you can fit a lot more"
			" in, but sooner or later you're going to run out, I hope...",
			false );

		// Cut the top in 4
		ui::controller::ColumnSlicer::Ratios const topColumnRatios = {
			{ 1.f / 10.f, false },
			{ 5.f / 10.f, true },
			{ 3.f / 10.f, true },
			{ 1.f / 10.f, false },
		};
		WeakPtr<ui::controller::ColumnSlicer> const topColumnSlicer =
			uiManager.AssignStrongController(
				rootRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					topColumnRatios ) );

		// Floating frame in middle left
		gfx::PPUCoord const leftFrameDimensions = { 128 - (16 + 8), 128 + 16 };
		WeakPtr<ui::controller::Floater> const leftFrameFloater =
			uiManager.AssignStrongController(
				topColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::Floater>::Create(
					leftFrameDimensions.x,
					leftFrameDimensions.y,
					ui::Justification::MiddleCenter ) );
		WeakPtr<ui::controller::BorderFrame> const leftFrame =
			uiManager.AssignStrongController(
				leftFrameFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::BorderFrame>::Create() );
		leftFrame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), { 4, 4 }, { 2, 0 } );

		// Floating frame in middle right
		gfx::PPUCoord const rightFrameDimensions = { 64 + 4, 128 + ( 16 - 4 ) };
		WeakPtr<ui::controller::Floater> const rightFrameFloater =
			uiManager.AssignStrongController(
				topColumnSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::Floater>::Create(
					rightFrameDimensions.x,
					rightFrameDimensions.y,
					ui::Justification::MiddleCenter ) );
		WeakPtr<ui::controller::BorderFrame> const rightFrame =
			uiManager.AssignStrongController(
				rightFrameFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::BorderFrame>::Create() );
		rightFrame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), { 4, 4 }, { 2, 0 } );

		// Left frame
		{
			// Cut in 2
			ui::controller::ColumnSlicer::Ratios const leftColumnRatios = {
				{ 2.f / 5.f, true },
				{ 3.f / 5.f, true },
			};
			WeakPtr<ui::controller::ColumnSlicer> const leftColumnSlicer =
				uiManager.AssignStrongController(
					leftFrame->GetChildContainerID(),
					DefaultCreator<ui::controller::ColumnSlicer>::Create(
						leftColumnRatios ) );

			static constexpr char const* kLeftText[] = {
				"CHARACTERNAME [E]",
				"Innate",
				"Genetics",
				"Phys Atk",
				"Phys Def",
				"Elem Atk",
				"Elem Def",
				"Balance",
				"Techniq",
				"Elem Pwr",
				"Breadth",
				"",
				"",
			};
			static constexpr char const* kLeftStatus[rftl::extent<decltype( kLeftText )>::value] = {
				"",
				"<  =======  >",
				"< F.AwokenC >",
				"- # # # # # +",
				"- # # # # # +",
				"- # # # # # +",
				"- # # # # # +",
				"- # # # # # +",
				"- # # # # # +",
				"- # # # # # +",
				"|---[]------|",
				"",
				"",
			};
			static constexpr char const* kLeftTextTags[rftl::extent<decltype( kLeftText )>::value] = {
				"O_Charname",
				"O_Innate",
				"O_Genetics",
				"O_PhysAtk",
				"O_PhysDef",
				"O_ElemAtk",
				"O_ElemDef",
				"O_Balance",
				"O_Techniq",
				"O_ElemPwr",
				"O_Breadth",
				"O_UNUSED_L1",
				"O_UNUSED_L2",
			};
			static constexpr char const* kLeftStatusTags[rftl::extent<decltype( kLeftText )>::value] = {
				"S_Charname",
				"S_Innate",
				"S_Genetics",
				"S_PhysAtk",
				"S_PhysDef",
				"S_ElemAtk",
				"S_ElemDef",
				"S_Balance",
				"S_Techniq",
				"S_ElemPwr",
				"S_Breadth",
				"S_UNUSED_L1",
				"S_UNUSED_L2",
			};

			// Create selections in the left frame
			WeakPtr<ui::controller::ListBox> const leftOptions =
				uiManager.AssignStrongController(
					leftColumnSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::ListBox>::Create(
						rftl::extent<decltype( kLeftText )>::value,
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			leftOptions->AddAsChildToFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode() );
			for( size_t i = 0; i < rftl::extent<decltype( kLeftText )>::value; i++ )
			{
				leftOptions->GetSlotController( i )->SetText( kLeftText[i] );
				uiManager.AssignLabel( leftOptions->GetSlotController( i )->GetContainerID(), kLeftTextTags[i] );
			}
			uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "O_Charname" )->SetIgnoreOverflow( true );

			// Create status in the left frame
			// HACK: Text as a mock-up for controls
			// TODO: This shouldn't be text
			WeakPtr<ui::controller::TextRows> const leftStatus =
				uiManager.AssignStrongController(
					leftColumnSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::TextRows>::Create(
						rftl::extent<decltype( kLeftStatus )>::value,
						ui::font::NarrowQuarterTileMono,
						ui::Justification::MiddleLeft,
						math::Color3f::kWhite ) );
			for( size_t i = 0; i < rftl::extent<decltype( kLeftStatus )>::value; i++ )
			{
				leftStatus->GetSlotController( i )->SetText( kLeftStatus[i] );
				uiManager.AssignLabel( leftStatus->GetSlotController( i )->GetContainerID(), kLeftStatusTags[i] );
			}
		}


		// Right frame
		{
			// Cut in 2 to make room for preview
			ui::controller::RowSlicer::Ratios const rightRowRatios = {
				{ 6.f / 20.f, true },
				{ 14.f / 20.f, true },
			};
			WeakPtr<ui::controller::RowSlicer> const rightRowSlicer =
				uiManager.AssignStrongController(
					rightFrame->GetChildContainerID(),
					DefaultCreator<ui::controller::RowSlicer>::Create(
						rightRowRatios ) );

			// Cut bottom in 2
			ui::controller::ColumnSlicer::Ratios const rightColumnRatios = {
				{ 11.f / 20.f, true },
				{ 9.f / 20.f, true },
			};
			WeakPtr<ui::controller::ColumnSlicer> const rightColumnSlicer =
				uiManager.AssignStrongController(
					rightRowSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::ColumnSlicer>::Create(
						rightColumnRatios ) );

			static constexpr char const* kRightText[] = {
				"S.Opaci", // Or F.Satur
				"S.PheoM", // Or F.PheoM
				"S.EuMel", // Or F.EuMel
				"H.Satur",
				"H.PheoM",
				"H.EuMel",
				"H.Dye",
				"Cloth",
				"C.Dye",
				"SAVE AND QUIT",
			};
			static constexpr char const* kRightStatus[rftl::extent<decltype( kRightText )>::value] = {
				"< ## >",
				"< ## >",
				"< ## >",
				"< ## >",
				"< ## >",
				"< ## >",
				"< ## >",
				"< ## >",
				"< ## >",
				"",
			};
			static constexpr char const* kRightTextTags[rftl::extent<decltype( kRightText )>::value] = {
				"O_Body1",
				"O_Body2",
				"O_Body3",
				"O_Hair1",
				"O_Hair2",
				"O_Hair3",
				"O_Hair4",
				"O_Cloth1",
				"O_Cloth2",
				"O_SAVE",
			};
			static constexpr char const* kRightStatusTags[rftl::extent<decltype( kRightText )>::value] = {
				"S_Body1",
				"S_Body2",
				"S_Body3",
				"S_Hair1",
				"S_Hair2",
				"S_Hair3",
				"S_Hair4",
				"S_Cloth1",
				"S_Cloth2",
				"S_UNUSED_R1",
			};

			// Create selections in the right frame
			WeakPtr<ui::controller::ListBox> const rightOptions =
				uiManager.AssignStrongController(
					rightColumnSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::ListBox>::Create(
						rftl::extent<decltype( kRightText )>::value,
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			rightOptions->AddAsSiblingAfterFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode().mFavoredChild );
			for( size_t i = 0; i < rftl::extent<decltype( kRightText )>::value; i++ )
			{
				rightOptions->GetSlotController( i )->SetText( kRightText[i] );
				uiManager.AssignLabel( rightOptions->GetSlotController( i )->GetContainerID(), kRightTextTags[i] );
			}
			uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "O_SAVE" )->SetIgnoreOverflow( true );

			// Create status in the right frame
			// HACK: Text as a mock-up for controls
			// TODO: This shouldn't be text
			WeakPtr<ui::controller::TextRows> const rightStatus =
				uiManager.AssignStrongController(
					rightColumnSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::TextRows>::Create(
						rftl::extent<decltype( kRightStatus )>::value,
						ui::font::NarrowQuarterTileMono,
						ui::Justification::MiddleLeft,
						math::Color3f::kWhite ) );
			for( size_t i = 0; i < rftl::extent<decltype( kRightStatus )>::value; i++ )
			{
				rightStatus->GetSlotController( i )->SetText( kRightStatus[i] );
				uiManager.AssignLabel( rightStatus->GetSlotController( i )->GetContainerID(), kRightStatusTags[i] );
			}
		}
	}
}



void TitleScreen_CharCreate::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_CharCreate::OnTick( AppStateTickContext& context )
{
	rftl::vector<ui::FocusEventType> const focusEvents = TitleScreen::GetInputToProcess();

	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		if( handled == false )
		{
			// Wasn't handled by general UI

			// Figure out the useful focus information
			WeakPtr<ui::FocusTreeNode const> currentFocus;
			WeakPtr<ui::FocusTarget const> currentFocusTarget;
			ui::ContainerID currentFocusContainerID = ui::kInvalidContainerID;
			{
				currentFocus = focusMan.GetFocusTree().GetCurrentFocus();
				if( currentFocus != nullptr )
				{
					currentFocusTarget = currentFocus->mFocusTarget;
				}
				if(currentFocusTarget != nullptr)
				{
					RF_ASSERT( currentFocusTarget->HasHardFocus() );
					currentFocusContainerID = currentFocusTarget->mContainerID;
				}
			}

			if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus)
			{
				if( currentFocusContainerID != ui::kInvalidContainerID )
				{
					// HACK: Disabled
					// TODO: Add a button
					//if( currentFocusContainerID == uiManager.GetContainerID( details::kReturnTag ) )
					//{
					//	context.mManager.RequestDeferredStateChange( id::TitleScreen_MainMenu );
					//}

					// HACK: Always back out to main menu if it's unhandled
					// TODO: Remove this once there's a proper button
					context.mManager.RequestDeferredStateChange( id::TitleScreen_MainMenu );
				}
			}
			else if( focusEvent == ui::focusevent::Command_NavigateUp )
			{
				// Assume we're at the edge of a pane, and swap to the edge of
				//  the other pane
				focusMan.GetMutableFocusTree().CycleRootFocusToPreviousChild( true );
				focusMan.HandleEvent( uiContext, ui::focusevent::Command_NavigateToLast );
			}
			else if( focusEvent == ui::focusevent::Command_NavigateDown )
			{
				// Assume we're at the edge of a pane, and swap to the edge of
				//  the other pane
				focusMan.GetMutableFocusTree().CycleRootFocusToNextChild( true );
				focusMan.HandleEvent( uiContext, ui::focusevent::Command_NavigateToFirst );
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}
