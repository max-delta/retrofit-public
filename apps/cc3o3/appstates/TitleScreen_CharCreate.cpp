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

		// Cut the top in 3
		ui::controller::ColumnSlicer::Ratios const topColumnRatios = {
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
		};
		WeakPtr<ui::controller::ColumnSlicer> const topColumnSlicer =
			uiManager.AssignStrongController(
				rootRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					topColumnRatios ) );

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
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}
