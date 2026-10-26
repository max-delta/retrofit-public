#include "stdafx.h"
#include "TitleScreen.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/input/InputFwd.h"
#include "cc3o3/appstates/titlescreen/TitleScreen_MainMenu.h"
#include "cc3o3/appstates/titlescreen/TitleScreen_CharCreate.h"
#include "cc3o3/appstates/titlescreen/TitleScreen_Options.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/scene/Scene.h"
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

	scene::Scene mScene = {};

	gfx::ppu::TileLayer* mBackgroundBack = nullptr;
	gfx::ppu::TileLayer* mBackgroundMid = nullptr;
	gfx::PPUTimer mTimerBack = {};
	gfx::PPUTimer mTimerMid = {};

	AppStateManager mAppStateManager;
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	// Load scene desc
	scene::Layer* backPtr = nullptr;
	scene::Layer* midPtr = nullptr;
	{
		scene::Scene& scene = internalState.mScene;

		file::VFSPath const& scenePath = paths::UIScenes().GetChild( "title_screen.oo" );

		scene = scene::Scene::LoadFromDesc( scenePath );
		scene.LoadResources( "main_menu" );

		backPtr = scene.FindFirstOfMutable( "back" );
		midPtr = scene.FindFirstOfMutable( "mid" );
		RFLOG_TEST_AND_FATAL( backPtr != nullptr, scenePath, RFCAT_CC3O3, "Expected 'back' layer" );
		RFLOG_TEST_AND_FATAL( midPtr != nullptr, scenePath, RFCAT_CC3O3, "Expected 'mid' layer" );
	}

	// Setup background
	{
		internalState.mBackgroundBack = &backPtr->mResources.mTileLayer;
		internalState.mBackgroundMid = &midPtr->mResources.mTileLayer;

		gfx::ppu::TileLayer& back = *internalState.mBackgroundBack;
		gfx::ppu::TileLayer& mid = *internalState.mBackgroundMid;
		gfx::PPUTimer& timerBack = internalState.mTimerBack;
		gfx::PPUTimer& timerMid = internalState.mTimerMid;

		back.mXCoord = 0;
		back.mYCoord = 0;
		back.mHorizontalWrapping = true;
		back.mVerticalWrapping = false;
		timerBack.mMaxTimeIndex = backPtr->mParallax;

		mid.mXCoord = 0;
		mid.mYCoord = 0;
		mid.mHorizontalWrapping = true;
		mid.mVerticalWrapping = false;
		timerMid.mMaxTimeIndex = midPtr->mParallax;
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
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	// Draw background
	{
		gfx::ppu::TileLayer& back = *internalState.mBackgroundBack;
		gfx::ppu::TileLayer& mid = *internalState.mBackgroundMid;
		gfx::PPUTimer& timerBack = internalState.mTimerBack;
		gfx::PPUTimer& timerMid = internalState.mTimerMid;

		constexpr auto parallax =
			[](
				gfx::ppu::PPUController const& ppu,
				gfx::ppu::TileLayer& tileLayer,
				gfx::PPUTimer& timer ) -> void //
		{
			timer.Animate( true, false );
			if( timer.IsFullZero() )
			{
				tileLayer.mXCoord--;
				if( tileLayer.mXCoord <= 0 )
				{
					tileLayer.mXCoord = ppu.CalculateTileLayerSize( tileLayer ).x;
				}
			}
		};
		parallax( ppu, back, timerBack );
		parallax( ppu, mid, timerMid );

		back.Animate();
		mid.Animate();

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
