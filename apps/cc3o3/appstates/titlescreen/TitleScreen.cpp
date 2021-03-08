#include "stdafx.h"
#include "TitleScreen.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/input/InputFwd.h"
#include "cc3o3/appstates/titlescreen/TitleScreen_MainMenu.h"
#include "cc3o3/appstates/titlescreen/TitleScreen_CharCreate.h"
#include "cc3o3/appstates/titlescreen/TitleScreen_Options.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/CommonPaths.h"

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


namespace RF::cc::appstate {
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
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( back, vfs, paths::BackgroundTilemaps().GetChild( "country_hills_back.csv" ) );
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
			bool const loadSuccess = gfx::TileLayerCSVLoader::LoadTiles( mid, vfs, paths::BackgroundTilemaps().GetChild( "country_hills_mid.csv" ) );
			RF_ASSERT( loadSuccess );
		}
	}


	// Start sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.AddState( appstate::id::TitleScreen_MainMenu, DefaultCreator<appstate::TitleScreen_MainMenu>::Create() );
	appStateMan.AddState( appstate::id::TitleScreen_CharCreate, DefaultCreator<appstate::TitleScreen_CharCreate>::Create() );
	appStateMan.AddState( appstate::id::TitleScreen_Options, DefaultCreator<appstate::TitleScreen_Options>::Create() );
	appStateMan.Start( appstate::GetFirstTitleScreenState() );
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
	appStateMan.ApplyDeferredStateChange();
}

///////////////////////////////////////////////////////////////////////////////
}
