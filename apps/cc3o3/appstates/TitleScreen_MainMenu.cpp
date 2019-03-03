#include "stdafx.h"
#include "TitleScreen_MainMenu.h"

#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/FramePackDisplay.h"
#include "GameUI/controllers/ListBox.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_MainMenu::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	// TODO: ???
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_MainMenu::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::FramePackManager const& framePackMan = *ppu.GetFramePackManager();
	ui::FontRegistry const& fontReg = *app::gFontRegistry;

	// TODO: Setup logic
	(void)internalState;

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
		logo->SetFramePack(
			framePackMan.GetManagedResourceIDFromResourceName( "cc303_composite_192" ),
			192,
			96 );

		// Menu floater in center bottom
		// TODO: Setup focus management
		rftl::vector<rftl::string> menuText;
		menuText.emplace_back( "Single player" );
		menuText.emplace_back( "Multiplayer" );
		menuText.emplace_back( "Character creator" );
		menuText.emplace_back( "Options" );
		menuText.emplace_back( "Exit to desktop" );
		uint8_t const menuFontHeight = fontReg.SelectBestFont( ui::font::LargeMenuSelection, 1 ).mFontHeight;
		constexpr gfx::PPUCoordElem kMenuEntryHitboxWidth = 160;
		constexpr gfx::PPUCoordElem kMenuEntryPadding = 6;
		gfx::PPUCoord const menuDimensions = {
			kMenuEntryHitboxWidth,
			math::integer_cast<gfx::PPUCoordElem>( ( menuFontHeight + kMenuEntryPadding ) * menuText.size() ) };
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
		menuEntries->AddAsChildToFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode() );

		// TODO: Basic debug in top left
		WeakPtr<ui::controller::TextLabel> const TODODebug =
			uiManager.AssignStrongController(
				leftRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		TODODebug->SetJustification( ui::Justification::TopLeft );
		TODODebug->SetFont( ui::font::MinSize );
		TODODebug->SetText( "DEBUG GOES HERE" );
		TODODebug->SetColor( math::Color3f::kBlack );

		// TODO: Build stamp in top right
		WeakPtr<ui::controller::TextLabel> const TODOBuild =
			uiManager.AssignStrongController(
				rightRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		TODOBuild->SetJustification( ui::Justification::TopRight );
		TODOBuild->SetFont( ui::font::MinSize );
		TODOBuild->SetText( "BUILD GOES HERE" );
		TODOBuild->SetColor( math::Color3f::kBlack );

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
		#if RF_IS_ALLOWED( RF_CONFIG_INTERNAL_BUILD_NOTICE )
		url->SetText( "INTERNAL USE ONLY" );
		url->SetColor( math::Color3f::kRed );
		url->SetBorder( true );
		#else
		url->SetText( "http://TODO" );
		url->SetColor( math::Color3f::kBlack );
		#endif
	}
}



void TitleScreen_MainMenu::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_MainMenu::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;

	// TODO: Run logic
	(void)internalState;
	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );
	static uint8_t HACK_roller1 = 0;
	if( HACK_roller1++ % 64 == 0 )
	{
		static uint8_t HACK_roller2 = 0;
		if( HACK_roller2++ % 10 < 5 )
		{
			focusMan.HandleEvent( uiContext, ui::focusevent::Command_NavigateDown );
		}
		else
		{
			focusMan.HandleEvent( uiContext, ui::focusevent::Command_NavigateUp );
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}
