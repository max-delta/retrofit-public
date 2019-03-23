#include "stdafx.h"
#include "TitleScreen_Options.h"

#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

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

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
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
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::FramePackManager const& framePackMan = *ppu.GetFramePackManager();
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

		// Cut the center in 3
		ui::controller::ColumnSlicer::Ratios const centerRowRatios = {
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const centerRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					centerRowRatios ) );

		// TODO
		(void)internalState;
		(void)fontReg;
		(void)ppu;
		(void)focusMan;
		(void)framePackMan;
	}
}



void TitleScreen_Options::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_Options::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;

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
					// TODO
					(void)internalState;
				}
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}
