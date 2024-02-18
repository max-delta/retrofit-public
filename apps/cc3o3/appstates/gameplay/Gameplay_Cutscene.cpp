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
#include "GameUI/controllers/TextLabel.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


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

		// TODO
		WeakPtr<ui::controller::TextLabel> const TODO =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::TextLabel>::Create() );
		TODO->SetJustification( ui::Justification::BottomCenter );
		TODO->SetFont( ui::font::LargeMenuHeader );
		TODO->SetText( "TODO" );
		TODO->SetColor( math::Color3f::kWhite );
		TODO->SetBorder( true );
		internalState.mTODO = TODO;
		( (void)focusMan );
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
