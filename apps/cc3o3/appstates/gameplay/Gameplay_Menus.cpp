#include "stdafx.h"
#include "Gameplay_Menus.h"

#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/UINavigation.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/Passthrough.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/FramePackDisplay.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Menus::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	WeakPtr<state::comp::UINavigation> mNavigation;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Menus::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	//gfx::FramePackManager const& framePackMan = *ppu.GetFramePackManager();

	// Find navigation component
	state::VariableIdentifier const localUIRoot( "localUI" );
	state::MutableObjectRef const localUI = state::FindMutableObjectByIdentifier( localUIRoot );
	RFLOG_TEST_AND_FATAL( localUI.IsSet(), nullptr, RFCAT_CC3O3, "Failed to find UI object" );
	internalState.mNavigation = localUI.GetMutableComponentInstanceT<state::comp::UINavigation>();
	RFLOG_TEST_AND_FATAL( internalState.mNavigation != nullptr, nullptr, RFCAT_CC3O3, "Failed to find navigation component" );

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		//ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
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

		// Cut the middle in 3
		ui::controller::RowSlicer::Ratios const middleRowRatios = {
			{ 1.f / 14.f, true },
			{ 12.f / 14.f, true },
			{ 1.f / 14.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const middleRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					middleRowRatios ) );

		// Header in top center
		WeakPtr<ui::controller::TextLabel> const mainHeader =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		mainHeader->SetJustification( ui::Justification::BottomCenter );
		mainHeader->SetFont( ui::font::LargeMenuHeader );
		mainHeader->SetText( "UNSET" );
		mainHeader->SetColor( math::Color3f::kWhite );
		mainHeader->SetBorder( true );

		// Controls in bottom right
		WeakPtr<ui::controller::TextLabel> const mainFooter =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		mainFooter->SetJustification( ui::Justification::TopRight );
		mainFooter->SetFont( ui::font::SmallMenuSelection );
		mainFooter->SetText( "UNSET" );
		mainFooter->SetColor( math::Color3f::kWhite );
		mainFooter->SetBorder( true );

		// Main sections are in the center
		ui::ContainerID const centerContainerID = middleRowSlicer->GetChildContainerID( 1 );

		// Loadout
		{
			// Passthrough
			WeakPtr<ui::controller::Passthrough> const loadoutPassthrough =
				uiManager.AssignStrongController(
					centerContainerID,
					DefaultCreator<ui::controller::Passthrough>::Create() );

			// Cut the section into columns
			ui::controller::ColumnSlicer::Ratios const loadoutColumnRatios = {
				{ 6.f / 18.f, true },
				{ 12.f / 18.f, true },
			};
			WeakPtr<ui::controller::ColumnSlicer> const loadoutColumnSlicer =
				uiManager.AssignStrongController(
					loadoutPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::ColumnSlicer>::Create(
						loadoutColumnRatios ) );

			// 3 character slots on left
			ui::controller::RowSlicer::Ratios const characterRowRatios = {
				{ 4.f / 12.f, true },
				{ 4.f / 12.f, true },
				{ 4.f / 12.f, true },
			};
			WeakPtr<ui::controller::RowSlicer> const characterRowSlicer =
				uiManager.AssignStrongController(
					loadoutColumnSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::RowSlicer>::Create(
						characterRowRatios ) );

			// Character slots
			for( size_t i_char = 0; i_char < 3; i_char++ )
			{
				// Frame
				WeakPtr<ui::controller::BorderFrame> const frame =
					uiManager.AssignStrongController(
						characterRowSlicer->GetChildContainerID( i_char ),
						DefaultCreator<ui::controller::BorderFrame>::Create() );
				frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );

				// 2 subsections for info and display
				ui::controller::ColumnSlicer::Ratios const charSlotColumnRatios = {
					{ 7.f / 10.f, true },
					{ 3.f / 10.f, true },
				};
				WeakPtr<ui::controller::ColumnSlicer> const charSlotColumnSlicer =
					uiManager.AssignStrongController(
						frame->GetChildContainerID(),
						DefaultCreator<ui::controller::ColumnSlicer>::Create(
							charSlotColumnRatios ) );

				// 3 info rows
				ui::controller::RowSlicer::Ratios const infoRowRatios = {
					{ 1.f / 3.f, true },
					{ 1.f / 3.f, true },
					{ 1.f / 3.f, true },
				};
				WeakPtr<ui::controller::RowSlicer> const infoRowSlicer =
					uiManager.AssignStrongController(
						charSlotColumnSlicer->GetChildContainerID( 0 ),
						DefaultCreator<ui::controller::RowSlicer>::Create(
							infoRowRatios ) );

				for( size_t i_info = 0; i_info < 3; i_info++ )
				{
					// Info
					WeakPtr<ui::controller::TextLabel> const info =
						uiManager.AssignStrongController(
							infoRowSlicer->GetChildContainerID( i_info ),
							DefaultCreator<ui::controller::TextLabel>::Create() );
					info->SetJustification( ui::Justification::MiddleLeft );
					info->SetFont( ui::font::LargeMenuText );
					info->SetText( "UNSET" );
					info->SetColor( math::Color3f::kWhite );
					info->SetBorder( true );
				}

				// Display
				//WeakPtr<ui::controller::FramePackDisplay> const display =
				//	uiManager.AssignStrongController(
				//		charSlotColumnSlicer->GetChildContainerID( 1 ),
				//		DefaultCreator<ui::controller::FramePackDisplay>::Create() );
				//display->SetJustification( ui::Justification::MiddleCenter );
			}

			// 2 subsections for element manager on right
			ui::controller::RowSlicer::Ratios const elementManagerRowRatios = {
				{ 3.f / 12.f, true },
				{ 9.f / 12.f, true },
			};
			WeakPtr<ui::controller::RowSlicer> const elementManagerRowSlicer =
				uiManager.AssignStrongController(
					loadoutColumnSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::RowSlicer>::Create(
						elementManagerRowRatios ) );
		}
	}
}



void Gameplay_Menus::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Menus::OnTick( AppStateTickContext& context )
{
	//InternalState& internalState = *mInternalState;
	//state::comp::UINavigation& navigation = *internalState.mNavigation;

	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Menus" );
}

///////////////////////////////////////////////////////////////////////////////
}}}
