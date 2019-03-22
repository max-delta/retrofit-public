#include "stdafx.h"
#include "TitleScreen.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/input/InputFwd.h"
#include "cc3o3/appstates/TitleScreen_MainMenu.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"
#include "GameAppState/AppStateTickContext.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"

#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/TileLayerCSVLoader.h"

#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	gfx::TileLayer mBackgroundBack = {};
	gfx::TileLayer mBackgroundMid = {};

	AppStateManager mAppStateManager;
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	file::VFS& vfs = *app::gVfs;

	file::VFSPath const tilemaps = file::VFS::kRoot.GetChild( "assets", "tilemaps" );

	// Setup background
	{
		gfx::TileLayer& back = internalState.mBackgroundBack;
		gfx::TileLayer& mid = internalState.mBackgroundMid;

		back = {};
		back.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_back_96" );
		back.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		back.mXCoord = 0;
		back.mYCoord = 0;
		back.mZLayer = 100;
		back.mWrapping = true;
		back.mLooping = true;
		back.mTimer.mMaxTimeIndex = 50;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( back, vfs, tilemaps.GetChild( "backgrounds", "country_hills_back.csv" ) );
			RF_ASSERT( loadSuccess );
		}

		mid = {};
		mid.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( "country_hills_mid_32" );
		mid.mTileZoomFactor = gfx::TileLayer::kTileZoomFactor_Normal;
		mid.mXCoord = 0;
		mid.mYCoord = 0;
		mid.mZLayer = 99;
		mid.mWrapping = true;
		mid.mLooping = true;
		mid.mTimer.mMaxTimeIndex = 10;
		{
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( mid, vfs, tilemaps.GetChild( "backgrounds", "country_hills_mid.csv" ) );
			RF_ASSERT( loadSuccess );
		}
	}


	// Start sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.AddState( appstate::id::TitleScreen_MainMenu, DefaultCreator<appstate::TitleScreen_MainMenu>::Create() );
	appStateMan.Start( appstate::id::TitleScreen_MainMenu );
}



void TitleScreen::OnExit( AppStateChangeContext& context )
{
	// Stop sub-states
	InternalState& internalState = *mInternalState;
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.Stop();

	mInternalState = nullptr;
}



void TitleScreen::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	// Draw background
	{
		gfx::TileLayer& back = internalState.mBackgroundBack;
		gfx::TileLayer& mid = internalState.mBackgroundMid;

		constexpr auto parralax = [](
			gfx::PPUController const& ppu,
			gfx::TileLayer& tileLayer ) -> void
		{
			tileLayer.Animate();
			if( tileLayer.mTimer.IsFullZero() )
			{
				tileLayer.mXCoord--;
				if( tileLayer.mXCoord <= 0 )
				{
					tileLayer.mXCoord = ppu.CalculateTileLayerSize( tileLayer ).x;
				}
			}
		};
		parralax( ppu, back );
		parralax( ppu, mid );

		ppu.DrawTileLayer( back );
		ppu.DrawTileLayer( mid );
	}

	// Tick sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.Tick( context.mCurrentTime, context.mElapsedTimeSinceLastTick );
}



rftl::vector<ui::FocusEventType> TitleScreen::GetInputToProcess()
{
	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	input::GameController const& controller = *controllerManager.GetGameController( input::player::P1, input::layer::MainMenu );

	// Fetch commands that were entered for this current frame
	rftl::vector<input::GameCommand> commands;
	rftl::virtual_back_inserter_iterator<input::GameCommand, decltype( commands )> parser( commands );
	controller.GetGameCommandStream( parser, time::FrameClock::now() );

	// Convert to UI
	rftl::vector<ui::FocusEventType> retVal;
	for( input::GameCommand const& command : commands )
	{
		switch( command.mType )
		{
			case input::command::game::UINavigateUp:
				retVal.emplace_back( ui::focusevent::Command_NavigateUp );
				break;
			case input::command::game::UINavigateDown:
				retVal.emplace_back( ui::focusevent::Command_NavigateDown );
				break;
			case input::command::game::UINavigateLeft:
				retVal.emplace_back( ui::focusevent::Command_NavigateLeft );
				break;
			case input::command::game::UINavigateRight:
				retVal.emplace_back( ui::focusevent::Command_NavigateRight );
				break;
			case input::command::game::UIActivateSelection:
				retVal.emplace_back( ui::focusevent::Command_ActivateCurrentFocus );
				break;
			case input::command::game::UICancelSelection:
				retVal.emplace_back( ui::focusevent::Command_CancelCurrentFocus );
				break;
			default:
				RF_DBGFAIL_MSG(
					"Encountered a command that doesn't have a UI mapping."
					" This input layer shouldn't have a mapping that the UI"
					" can't handle." );
				break;
		}
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}}
