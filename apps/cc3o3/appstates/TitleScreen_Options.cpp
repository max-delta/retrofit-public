#include "stdafx.h"
#include "TitleScreen_Options.h"

#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/ListBox.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr char kOpt1Tag[] = "OPT1";
static constexpr char kOpt2Tag[] = "OPT2";
static constexpr char kOpt3Tag[] = "OPT3";
static constexpr char kOpt4Tag[] = "OPT4";
static constexpr char kOpt5Tag[] = "OPT5";
static constexpr char kOpt6Tag[] = "OPT6";
static constexpr char kOpt7Tag[] = "OPT7";
static constexpr char kOpt8Tag[] = "OPT8";
static constexpr char kOpt9Tag[] = "OPT9";
static constexpr char kOpt10Tag[] = "OPT10";
static constexpr char kOpt11Tag[] = "OPT11";
static constexpr char kOpt12Tag[] = "OPT12";
static constexpr char kUnusedTag[] = "UNUSED";
static constexpr char kReturnTag[] = "RETURN";

}
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_Options::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	ui::ContainerID mSinglePlayer = ui::kInvalidContainerID;
	ui::ContainerID mMultiplayer = ui::kInvalidContainerID;
	ui::ContainerID mCharacterCreator = ui::kInvalidContainerID;
	ui::ContainerID mOptions = ui::kInvalidContainerID;
	ui::ContainerID mExitToDesktop = ui::kInvalidContainerID;
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_Options::OnEnter( AppStateChangeContext& context )
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

		// Cut the whole screen into columns
		ui::controller::ColumnSlicer::Ratios const rootColumnRatios = {
			{ 1.f / 20.f, false },
			{ 18.f / 20.f, true },
			{ 1.f / 20.f, false },
		};
		WeakPtr<ui::controller::ColumnSlicer> const rootColumnSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					rootColumnRatios ) );

		// Cut the center in 4
		ui::controller::ColumnSlicer::Ratios const centerRowRatios = {
			{ 1.f / 14.f, false },
			{ 1.f / 14.f, true },
			{ 11.f / 14.f, true },
			{ 1.f / 14.f, false },
		};
		WeakPtr<ui::controller::RowSlicer> const centerRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					centerRowRatios ) );

		// Header in top center
		WeakPtr<ui::controller::TextLabel> const header =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		header->SetJustification( ui::Justification::BottomCenter );
		header->SetFont( ui::font::LargeMenuHeader );
		header->SetText( "Options" );
		header->SetColor( math::Color3f::kWhite );
		header->SetBorder( true );

		// Frame in middle center
		WeakPtr<ui::controller::BorderFrame> const frame =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::BorderFrame>::Create() );
		frame->SetTileset( tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), 4, 4 );

		// Cut the frame into columns
		ui::controller::ColumnSlicer::Ratios const frameColumnRatios = {
			{ 1.f / 2.f, true },
			{ 1.f / 2.f, true },
		};
		WeakPtr<ui::controller::ColumnSlicer> const frameColumnSlicer =
			uiManager.AssignStrongController(
				frame->GetChildContainerID(),
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					frameColumnRatios ) );

		// Create menu selections in the left columns
		rftl::vector<rftl::string> leftOptionsText;
		leftOptionsText.emplace_back( "Menu option text for slot 1" );
		leftOptionsText.emplace_back( "Menu option text for slot 2" );
		leftOptionsText.emplace_back( "Menu option text for slot 3" );
		leftOptionsText.emplace_back( "Menu option text for slot 4" );
		leftOptionsText.emplace_back( "Menu option text for slot 5" );
		leftOptionsText.emplace_back( "Menu option text for slot 6" );
		leftOptionsText.emplace_back( "Menu option text for slot 7" );
		leftOptionsText.emplace_back( "Menu option text for slot 8" );
		leftOptionsText.emplace_back( "Menu option text for slot 9" );
		leftOptionsText.emplace_back( "Menu option text for slot 10" );
		leftOptionsText.emplace_back( "Menu option text for slot 11" );
		leftOptionsText.emplace_back( "Menu option text for slot 12" );
		leftOptionsText.emplace_back(); // Unused
		leftOptionsText.emplace_back( "Return to main menu" );
		WeakPtr<ui::controller::ListBox> const leftOptions =
			uiManager.AssignStrongController(
				frameColumnSlicer->GetChildContainerID(0),
				DefaultCreator<ui::controller::ListBox>::Create(
					leftOptionsText.size(),
					ui::font::SmallMenuSelection,
					ui::Justification::MiddleLeft,
					math::Color3f::kGray50,
					math::Color3f::kWhite,
					math::Color3f::kYellow ) );
		leftOptions->SetText( leftOptionsText );
		leftOptions->AddAsChildToFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode() );
		uiManager.AssignLabel( leftOptions->GetSlotController( 0 )->GetContainerID(), details::kOpt1Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 1 )->GetContainerID(), details::kOpt2Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 2 )->GetContainerID(), details::kOpt3Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 3 )->GetContainerID(), details::kOpt4Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 4 )->GetContainerID(), details::kOpt5Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 5 )->GetContainerID(), details::kOpt6Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 6 )->GetContainerID(), details::kOpt7Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 7 )->GetContainerID(), details::kOpt8Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 8 )->GetContainerID(), details::kOpt9Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 9 )->GetContainerID(), details::kOpt10Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 10 )->GetContainerID(), details::kOpt11Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 11 )->GetContainerID(), details::kOpt12Tag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 12 )->GetContainerID(), details::kUnusedTag );
		uiManager.AssignLabel( leftOptions->GetSlotController( 13 )->GetContainerID(), details::kReturnTag );
	}
}



void TitleScreen_Options::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_Options::OnTick( AppStateTickContext& context )
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
					if( currentFocusContainerID == uiManager.GetContainerID( details::kReturnTag ) )
					{
						context.mManager.RequestDeferredStateChange( id::TitleScreen_MainMenu );
					}
				}
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}
