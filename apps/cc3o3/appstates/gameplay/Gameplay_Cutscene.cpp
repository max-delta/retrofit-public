#include "stdafx.h"
#include "Gameplay_Cutscene.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/campaign/CampaignManager.h"
#include "cc3o3/ui/LocalizationHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusManager.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/AspectColumnSlicer.h"
#include "GameUI/controllers/Clamper.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/MessageBox.h"
#include "GameUI/controllers/TextLabel.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"
#include "core/meta/IntegerPromotion.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Cutscene::InternalState
{
	RF_NO_COPY( InternalState );

public:
	InternalState() = default;

public:
	WeakPtr<ui::controller::TextLabel> mTODO;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Cutscene::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	// Setup cutscene
	{
		campaign::CampaignManager& campaign = *gCampaignManager;

		// TODO: Load cutscene
		( (void)campaign );
	}

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Constrain the aspect ratio
		ui::controller::AspectColumnSlicer::Ratio const aspectRatio( 12, 7 );
		ui::controller::AspectColumnSlicer::Enableds const aspectEnableds = {
			false,
			true,
			false,
		};
		WeakPtr<ui::controller::AspectColumnSlicer> const aspectSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::AspectColumnSlicer>::Create(
					aspectRatio,
					aspectEnableds ) );

		// Cut the center in 5
		ui::controller::RowSlicer::Ratios const centerRowRatios = {
			{ 1.f / 28.f, false },
			{ 8.f / 28.f, true },
			{ 10.f / 28.f, false },
			{ 8.f / 28.f, true },
			{ 1.f / 28.f, false },
		};
		WeakPtr<ui::controller::RowSlicer> const centerRowSlicer =
			uiManager.AssignStrongController(
				aspectSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					centerRowRatios ) );

		// Put clampers on the top and bottom
		static constexpr ui::controller::Clamper::Params kClamperParams = {
			.subtractWidth = angry_cast<gfx::ppu::CoordElem>( gfx::ppu::kTileSize / 2 ),
			.maxHeight = angry_cast<gfx::ppu::CoordElem>( gfx::ppu::kTileSize * 2 ),
			.divisibleByWidth = angry_cast<gfx::ppu::CoordElem>( gfx::ppu::kTileSize / 4 ),
			.divisibleByHeight = angry_cast<gfx::ppu::CoordElem>( gfx::ppu::kTileSize / 4 ) };
		WeakPtr<ui::controller::Clamper> const topClamper =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::Clamper>::Create(
					kClamperParams,
					ui::Justification::TopCenter ) );
		WeakPtr<ui::controller::Clamper> const bottomClamper =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 3 ),
				DefaultCreator<ui::controller::Clamper>::Create(
					kClamperParams,
					ui::Justification::BottomCenter ) );

		// TODO
		WeakPtr<ui::controller::TextLabel> const TODO =
			uiManager.AssignStrongController(
				topClamper->GetChildContainerID(),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		TODO->SetJustification( ui::Justification::MiddleCenter );
		TODO->SetFont( ui::font::LargeMenuHeader );
		TODO->SetText( "TODO" );
		TODO->SetColor( math::Color3f::kWhite );
		TODO->SetBorder( true );
		internalState.mTODO = TODO;
		( (void)focusMan );

		// TODO
		WeakPtr<ui::controller::MessageBox> const TODO2 =
			uiManager.AssignStrongController(
				bottomClamper->GetChildContainerID(),
				DefaultCreator<ui::controller::MessageBox>::Create(
					4u,
					ui::font::MessageBox,
					ui::Justification::MiddleLeft,
					math::Color3f::kWhite,
					ui::GetBreakableChars() ) );
		static constexpr char kTODOText[] =
			"This is some really long text that isn't going to fit on one line."
			" In fact, it probably won't even fit in one text box."
			" This is likely going to be truncated multiple ways, and require"
			" pagination support so a user can work their way through the full"
			" lenghty text.";
		TODO2->SetText( kTODOText, false );
		TODO2->SetAnimationSpeed( 1 );
		TODO2->SetFastForwardEvent( ui::focusevent::Command_ActivateCurrentFocus );
		TODO2->AddAsChildToFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode() );
	}
}



void Gameplay_Cutscene::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Cutscene::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
	campaign::CampaignManager& campaign = *gCampaignManager;

	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );

	// Process menu actions
	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetGameMenuInputToProcess( InputHelpers::GetSinglePlayer() );
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		focusMan.UpdateHardFocus( uiContext );

		// Figure out the useful focus information
		ui::ContainerID const currentFocusContainerID = focusMan.GetFocusTree().GetCurrentFocusContainerID();

		if( handled )
		{
			// Handled by the normal UI
		}
		else
		{
			// Wasn't handled by general UI

			// TODO: What isn't going to be handled by the normal UI already?
			if( currentFocusContainerID != ui::kInvalidContainerID )
			{
				// TODO: ???
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}

	// TODO
	( (void)internalState );
	( (void)campaign );
	app::gGraphics->DebugDrawText( gfx::ppu::Coord( 128, 32 ), "TODO: Cutscene" );
}

///////////////////////////////////////////////////////////////////////////////
}
