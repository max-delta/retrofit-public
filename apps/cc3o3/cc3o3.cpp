#include "stdafx.h"
#include "cc3o3.h"

#include "cc3o3/appstates/Boot.h"
#include "cc3o3/appstates/InitialLoading.h"
#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/appstates/DevTestCombatCharts.h"
#include "cc3o3/appstates/DevTestRollback.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/DeveloperHud.h"
#include "cc3o3/input/InputFwd.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/time/TimeFwd.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"
#include "GameInput/ControllerManager.h"
#include "GameUI/ContainerManager.h"

#include "PPU/PPUController.h"

#include "Timing/FrameClock.h"
#include "Rollback/RollbackManager.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

constexpr bool kAllowDebug = true;

constexpr bool kDebugGrid = kAllowDebug && true;
constexpr bool kDebugUI = kAllowDebug && true;
constexpr bool kDebugUIScaler = kAllowDebug && false;
constexpr bool kDebugHideZoomFactor = kAllowDebug && false;
constexpr bool kDebugSuppressRender = kAllowDebug && false;

constexpr bool kAllowDevTests = true;
constexpr bool kAllowDeveloperHud = true;

static appstate::AppStateManager sAppStateManager;

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	SystemStartup();

	sAppStateManager.AddState( appstate::id::Boot, DefaultCreator<appstate::Boot>::Create() );
	sAppStateManager.AddState( appstate::id::InitialLoading, DefaultCreator<appstate::InitialLoading>::Create() );
	sAppStateManager.AddState( appstate::id::TitleScreen, DefaultCreator<appstate::TitleScreen>::Create() );
	if( kAllowDevTests )
	{
		sAppStateManager.AddState( appstate::id::DevTestCombatCharts, DefaultCreator<appstate::DevTestCombatCharts>::Create() );
		sAppStateManager.AddState( appstate::id::DevTestRollback, DefaultCreator<appstate::DevTestRollback>::Create() );
	}

	sAppStateManager.Start( appstate::GetBootState() );

	app::gGraphics->DebugSetGridEnabled( kDebugGrid );
	app::gGraphics->HideZoomFactor( kDebugHideZoomFactor );

	// Leave atleast a pixel around all edges of the screen
	app::gUiManager->SetRootAABBReduction( 1 );

	if( kDebugUIScaler )
	{
		app::gUiManager->SetDebugAABBReduction( 2 );
	}

	if( kAllowDeveloperHud )
	{
		developer::Startup();
	}
}



void ProcessFrame()
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;
	input::ControllerManager& controllerManager = *app::gInputControllerManager;

	// The head of rollback should be at the end of the previous frame
	RFLOG_TEST_AND_FATAL(
		rollMan.GetHeadClock() < time::FrameClock::now(),
		nullptr,
		RFCAT_CC3O3,
		"Rollback head is ahead of frame" );
	RFLOG_TEST_AND_FATAL(
		rollMan.GetHeadClock() == time::FrameClock::now() - time::kSimulationFrameDuration,
		nullptr,
		RFCAT_CC3O3,
		"Rollback head not on the previous frame" );
	rollMan.SetHeadClock( time::FrameClock::now() );

	// HACK: Tick hard-coded input
	// TODO: Have an input processing tree that handles dependency-based update
	//  logic for all the controllers
	input::HardcodedTick();

	if( kAllowDeveloperHud )
	{
		// Developer input is global, and processed seperately from app states
		developer::ProcessInput();
	}

	if( rollMan.GetHeadClock() < time::FrameClock::now() )
	{
		// The head has moved backwards in time since we started the frame
		if constexpr( false )
		{
			// This was caused by a correction, so we need to re-sim the
			//  the previous frames before we can simulate the current frame
			// TODO: Re-sim logic
		}
		else if constexpr( false )
		{
			// This was caused by a replay, so we need to stop processing any
			//  true input while we wait for the replay to catch up to the
			//  current true time
			// TODO: Replay logic
		}
		else
		{
			// This was caused by a destructive state load, so we need to
			//  revise what we think 'now' means
			RFLOG_INFO( nullptr, RFCAT_CC3O3, "Rollback detected for load state, truncating state" );

			// Rewind all rollback systems so that they can be written to again
			rollMan.RewindAllStreams( rollMan.GetHeadClock() );

			// Make sure we've truncated any state that is rollback-sensitive,
			//  but not rollback-aware
			controllerManager.TruncateAllRegisteredGameControllers( time::CommonClock::time_point(), rollMan.GetHeadClock() );

			// The head clock is presumed to be valid, so we should begin
			//  processing the frame afterwards (which is now what we're
			//  considering the current frame)
			time::CommonClock::time_point const frameAfterHead = rollMan.GetHeadClock() + time::kSimulationFrameDuration;
			time::FrameClock::set_time( frameAfterHead );
			rollMan.SetHeadClock( frameAfterHead );
		}
	}

	sAppStateManager.Tick( time::FrameClock::now(), time::kSimulationFrameDuration );
	sAppStateManager.ApplyDeferredStateChange();

	// Commit all frames that are ready
	rollback::InclusiveTimeRange const commitRange = rollMan.GetFramesReadyToCommit();
	rollMan.CommitFrames( commitRange, commitRange.second + time::kSimulationFrameDuration );

	// For locally-owned streams, lock the read heads to after the last commit
	//  and the write heads to the next frame
	// HACK: Advance hard-coded input
	// TODO: Have a manifest for what is locally-owned
	input::HardcodedAdvance(
		commitRange.second + time::kSimulationFrameDuration,
		time::FrameClock::now() + time::kSimulationFrameDuration );

	ui::ContainerManager& uiMan = *app::gUiManager;
	uiMan.RecalcRootContainer();
	uiMan.ProcessRecalcs();
	if( kDebugUI )
	{
		uiMan.DebugRender( kDebugSuppressRender );
	}
	if( kDebugSuppressRender == false )
	{
		uiMan.Render();
	}

	if( kAllowDeveloperHud )
	{
		developer::RenderHud();
	}
}



void Shutdown()
{
	if( kAllowDeveloperHud )
	{
		developer::Shutdown();
	}

	sAppStateManager.Stop();

	SystemShutdown();
}

///////////////////////////////////////////////////////////////////////////////
}}
