#include "stdafx.h"
#include "TitleScreen_MainMenu.h"

#include "cc3o3/appstates/titlescreen/TitleScreen.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/cc3o3.h"

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
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/TextRows.h"
#include "GameUI/controllers/FramePackDisplay.h"
#include "GameUI/controllers/ListBox.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr char kSinglePlayerTag[] = "SPLAY";
static constexpr char kMultiplayerTag[] = "MPLAY";
static constexpr char kCharacterCreatorTag[] = "CHAR";
static constexpr char kOptionsTag[] = "OPT";
static constexpr char kExitToDesktopTag[] = "EXIT";

}
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_MainMenu::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_MainMenu::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	gfx::ppu::PPUController const& ppu = *app::gGraphics;
	gfx::ppu::FramePackManager const& framePackMan = *ppu.GetFramePackManager();
	ui::FontRegistry const& fontReg = *app::gFontRegistry;

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Cut the whole screen into columns
		ui::controller::ColumnSlicer::Ratios const rootColumnRatios = {
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
		};
		WeakPtr<ui::controller::ColumnSlicer> const rootColumnSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					rootColumnRatios ) );

		// Cut the left in 2
		ui::controller::ColumnSlicer::Ratios const leftRowRatios = {
			{ 1.f / 2.f, true },
			{ 1.f / 2.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const leftRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					leftRowRatios ) );

		// Cut the center in 2
		ui::controller::ColumnSlicer::Ratios const centerRowRatios = {
			{ 1.f / 2.f, true },
			{ 1.f / 2.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const centerRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					centerRowRatios ) );

		// Cut the right in 2
		ui::controller::ColumnSlicer::Ratios const rightRowRatios = {
			{ 1.f / 2.f, true },
			{ 1.f / 2.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const rightRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					rightRowRatios ) );

		// Logo in center top
		WeakPtr<ui::controller::FramePackDisplay> const logo =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::FramePackDisplay>::Create() );
		logo->SetJustification( ui::Justification::MiddleCenter );
		gfx::ppu::ManagedFramePackID const logoFpackID = framePackMan.GetManagedResourceIDFromResourceName( "cc303_composite_192" );
		gfx::ppu::FramePackBase const& logoFpack = *framePackMan.GetResourceFromManagedResourceID( logoFpackID );
		logo->SetFramePack(
			logoFpackID,
			logoFpack.CalculateTimeIndexBoundary(),
			192,
			96 );

		// Menu floater in center bottom
		rftl::vector<rftl::string> menuText;
		menuText.emplace_back( ui::LocalizeKey( "$mainmenu_splay" ) );
		menuText.emplace_back( ui::LocalizeKey( "$mainmenu_mplay" ) );
		menuText.emplace_back( ui::LocalizeKey( "$mainmenu_char" ) );
		menuText.emplace_back( ui::LocalizeKey( "$mainmenu_opt" ) );
		menuText.emplace_back( ui::LocalizeKey( "$mainmenu_exit" ) );
		uint8_t const menuFontHeight = fontReg.SelectBestFont( ui::font::LargeMenuSelection, 1 ).mFontHeight;
		constexpr gfx::ppu::PPUCoordElem kMenuEntryHitboxWidth = 160;
		constexpr gfx::ppu::PPUCoordElem kMenuEntryPadding = 6;
		gfx::ppu::PPUCoord const menuDimensions = {
			kMenuEntryHitboxWidth,
			math::integer_cast<gfx::ppu::PPUCoordElem>( ( menuFontHeight + kMenuEntryPadding ) * menuText.size() )
		};
		WeakPtr<ui::controller::Floater> const menuPane =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::Floater>::Create(
					menuDimensions.x,
					menuDimensions.y,
					ui::Justification::MiddleCenter ) );
		WeakPtr<ui::controller::ListBox> const menuEntries =
			uiManager.AssignStrongController(
				menuPane->GetChildContainerID(),
				DefaultCreator<ui::controller::ListBox>::Create(
					menuText.size(),
					ui::font::LargeMenuSelection ) );
		menuEntries->SetText( menuText );
		menuEntries->SetWrapping( true );
		menuEntries->AddAsChildToFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode() );
		uiManager.AssignLabel( menuEntries->GetSlotController( 0 )->GetContainerID(), details::kSinglePlayerTag );
		uiManager.AssignLabel( menuEntries->GetSlotController( 1 )->GetContainerID(), details::kMultiplayerTag );
		uiManager.AssignLabel( menuEntries->GetSlotController( 2 )->GetContainerID(), details::kCharacterCreatorTag );
		uiManager.AssignLabel( menuEntries->GetSlotController( 3 )->GetContainerID(), details::kOptionsTag );
		uiManager.AssignLabel( menuEntries->GetSlotController( 4 )->GetContainerID(), details::kExitToDesktopTag );

		// TODO: Basic debug in top left
		WeakPtr<ui::controller::TextLabel> const TODODebug =
			uiManager.AssignStrongController(
				leftRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		TODODebug->SetJustification( ui::Justification::TopLeft );
		TODODebug->SetFont( ui::font::MinSize );
		TODODebug->SetText( "DEBUG GOES HERE" );
		TODODebug->SetColor( math::Color3f::kBlack );

		// Build stamp floater in top right
		rftl::vector<rftl::string> buildText;
		buildText.emplace_back( buildstamp::kTime );
		buildText.emplace_back( buildstamp::kType );
		buildText.emplace_back( buildstamp::kVersion );
		buildText.emplace_back( buildstamp::kSource );
		buildText.emplace_back( compiler::kToolDesc );
		uint8_t const buildFontHeight = fontReg.SelectBestFont( ui::font::MinSize, 1 ).mFontHeight;
		constexpr gfx::ppu::PPUCoordElem kBuildEntryHitboxWidth = 80;
		constexpr gfx::ppu::PPUCoordElem kBuildEntryPadding = 1;
		gfx::ppu::PPUCoord const buildDimensions = {
			kBuildEntryHitboxWidth,
			math::integer_cast<gfx::ppu::PPUCoordElem>( ( buildFontHeight + kBuildEntryPadding ) * buildText.size() )
		};
		WeakPtr<ui::controller::Floater> const buildPane =
			uiManager.AssignStrongController(
				rightRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::Floater>::Create(
					buildDimensions.x,
					buildDimensions.y,
					ui::Justification::TopRight ) );
		WeakPtr<ui::controller::TextRows> const buildLabels =
			uiManager.AssignStrongController(
				buildPane->GetChildContainerID(),
				DefaultCreator<ui::controller::TextRows>::Create(
					buildText.size(),
					ui::font::MinSize,
					ui::Justification::MiddleRight,
					math::Color3f::kBlack ) );
		buildLabels->SetText( buildText );

		// Copyright in bottom left
		WeakPtr<ui::controller::TextLabel> const copyright =
			uiManager.AssignStrongController(
				leftRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		copyright->SetJustification( ui::Justification::BottomLeft );
		copyright->SetFont( ui::font::MinSize );
		copyright->SetText( "(C) Max Delta" );
		copyright->SetColor( math::Color3f::kBlack );

		// Url in bottom right
		WeakPtr<ui::controller::TextLabel> const url =
			uiManager.AssignStrongController(
				rightRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		url->SetJustification( ui::Justification::BottomRight );
		url->SetFont( ui::font::MinSize );
		if constexpr( config::kInternalBuildNotice )
		{
			url->SetText( "INTERNAL USE ONLY" );
			url->SetColor( math::Color3f::kRed );
			url->SetBorder( true );
		}
		else
		{
			url->SetText( "http://TODO" );
			url->SetColor( math::Color3f::kBlack );
		}
	}
}



void TitleScreen_MainMenu::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_MainMenu::OnTick( AppStateTickContext& context )
{
	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();

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
			ui::ContainerID const currentFocusContainerID = focusMan.GetFocusTree().GetCurrentFocusContainerID();

			if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
			{
				if( currentFocusContainerID != ui::kInvalidContainerID )
				{
					if( currentFocusContainerID == uiManager.GetContainerID( details::kSinglePlayerTag ) )
					{
						RequestGlobalDeferredStateChange( id::Gameplay );
					}
					else if( currentFocusContainerID == uiManager.GetContainerID( details::kMultiplayerTag ) )
					{
						RequestGlobalDeferredStateChange( id::DevTestLobby );
					}
					else if( currentFocusContainerID == uiManager.GetContainerID( details::kCharacterCreatorTag ) )
					{
						context.mManager.RequestDeferredStateChange( id::TitleScreen_CharCreate );
					}
					else if( currentFocusContainerID == uiManager.GetContainerID( details::kOptionsTag ) )
					{
						context.mManager.RequestDeferredStateChange( id::TitleScreen_Options );
					}
					else if( currentFocusContainerID == uiManager.GetContainerID( details::kExitToDesktopTag ) )
					{
						app::gShouldExit = true;
					}
				}
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
