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
	input::HardcodedRawTick();

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
			input::DebugClearTestInput();

			// The head clock is presumed to be valid, so we should begin
			//  processing the frame afterwards (which is now what we're
			//  considering the current frame)
			time::CommonClock::time_point const frameAfterHead = rollMan.GetHeadClock() + time::kSimulationFrameDuration;
			time::FrameClock::set_time( frameAfterHead );
			rollMan.SetHeadClock( frameAfterHead );
		}
	}

	// HACK: Tick test input
	// TODO: Have some sync manager that can submit queued up input from debug
	//  as well as more legitimate systems like networking
	input::DebugSubmitTestInput();

	// We're about to do some really weird stuff to time, so we need to make
	//  sure we don't lose track of what frame we're ultimately trying to
	//  put together
	time::CommonClock::time_point const currentTrueFrame = time::FrameClock::now();
	time::CommonClock::time_point const previousTrueFrame = currentTrueFrame - time::kSimulationFrameDuration;
	RF_ASSERT( previousTrueFrame < currentTrueFrame );

	// Figure out what type of simulation we need to perform
	enum class SimulationMode : uint8_t
	{
		Invalid = 0,
		OnRailsReplay,
		SingleFrameSimulate,
		RollbackAndSimulate,
		StallSimulation
	} simulationMode = SimulationMode::Invalid;
	time::CommonClock::time_point const preFrameCommitHead = rollMan.GetMaxCommitHead();
	rollback::InclusiveTimeRange preFrameCommitRange = { preFrameCommitHead, preFrameCommitHead };
	if( preFrameCommitHead >= currentTrueFrame )
	{
		// Committed input is in the future, which means the input for this
		//  frame is pre-determined and we just need to simulate it
		// NOTE: The current true frame counts as the future, since we haven't
		//  yet simulated it (to our knowledge)
		simulationMode = SimulationMode::OnRailsReplay;
	}
	else
	{
		RF_ASSERT( preFrameCommitHead <= previousTrueFrame );

		// Committed input is behind the current frame, maybe there's some
		//  uncommitted input in the past that is ready to be committed, and so
		//  we need to re-simulate those frames?
		if( rollMan.GetUncommittedStreams().empty() )
		{
			// We don't even have uncommitted streams, so the rollback
			//  machinery is currently inert
			// NOTE: This often happens on application startup while the
			//  relevant systems are still spinning up
			simulationMode = SimulationMode::SingleFrameSimulate;
			preFrameCommitRange = { preFrameCommitHead, previousTrueFrame };
		}
		else
		{
			// See if there's anything to commit between our commit head and
			//  the current frame
			preFrameCommitRange = rollMan.GetFramesReadyToCommit();
			RF_ASSERT( preFrameCommitHead <= preFrameCommitRange.first );
			RF_ASSERT( preFrameCommitRange.second < currentTrueFrame );
			if( preFrameCommitRange.second <= preFrameCommitHead )
			{
				// Nothing new to commit
				if( preFrameCommitHead == previousTrueFrame )
				{
					// We've committed the previous frame, so we're not yet
					//  expecting any rollbacks
					simulationMode = SimulationMode::SingleFrameSimulate;
				}
				else
				{
					// We're simulating multiple frames ahead of our committed
					//  input, so we're likely going to get a rollback and have
					//  to re-simulate those frames
					RF_ASSERT( preFrameCommitHead < previousTrueFrame );
					time::CommonClock::duration const timeSinceLastCommit =
						previousTrueFrame - preFrameCommitHead;
					static constexpr time::CommonClock::duration kMaxRollbackDuration =
						rollback::kMaxChangesInWindow * time::kSimulationFrameDuration;
					if( timeSinceLastCommit < kMaxRollbackDuration )
					{
						// We're still not so far ahead that we can't rollback,
						//  so it's safe to keep predicting inputs for now and
						//  hope for the best
						simulationMode = SimulationMode::SingleFrameSimulate;
					}
					else
					{
						// Uh-oh, if we keep simulating we likely won't be able
						//  to recover, so we need to stall out until the input
						//  streams can commit or are disconnected
						simulationMode = SimulationMode::StallSimulation;
					}
				}
			}
			else
			{
				// There's new inputs that theoretically need to re-simulated,
				//  but they might be no-op inputs, and thus safely ignorable
				RF_TODO_ANNOTATION( "Check for no-ops" );
				if constexpr( false )
				{
					// All the inputs are no-ops, so we've already effectively
					//  simulated them by just assuming they were no-ops in
					//  previous frames
					simulationMode = SimulationMode::SingleFrameSimulate;
				}
				else
				{
					// Some inputs were discovered in frames that we've already
					//  simulated, where we mis-predicted what the input was,
					//  and so have to go back and re-simulate that frame, and
					//  all other frames after it, before we can simulate the
					//  current frame
					simulationMode = SimulationMode::RollbackAndSimulate;
				}
			}
		}
	}
	RF_ASSERT( simulationMode != SimulationMode::Invalid );

	// For locally-owned streams, we'll want to manipulate read/write heads
	// HACK: Advance hard-coded input
	RF_TODO_ANNOTATION( "Have a manifest for what is locally-owned" );
	(void)( &input::HardcodedAdvance );

	// Commit all frames that are ready
	rollMan.CommitFrames( preFrameCommitRange, preFrameCommitRange.second );

	// Lock read heads to after the last commit, and write heads to the start
	//  of the current frame
	input::HardcodedAdvance(
		preFrameCommitRange.second + time::kSimulationFrameDuration,
		time::FrameClock::now() );

	if( simulationMode == SimulationMode::RollbackAndSimulate )
	{
		gfx::PPUController& ppu = *app::gGraphics;

		// Suppress any draw requests while performing rollback re-simulation
		ppu.SuppressDrawRequests( true );

		// TODO: Rollback and simulate each frame up to the current true frame

		// Restore graphics before ticking true frame
		ppu.SuppressDrawRequests( false );
	}

	// HACK: Tick hard-coded input
	// TODO: Have an input processing tree that handles dependency-based update
	//  logic for all the controllers
	input::HardcodedRollbackTick();

	if( simulationMode == SimulationMode::StallSimulation )
	{
		// HACK: Take a snapshot of the state before tick, so we can restore it
		//  at end of frame
		// TODO: Instead, we should just truncate any state from this frame at
		//  the end of the frame, since that should theoretically be a much
		//  cheaper option
		rollMan.TakeManualSnapshot( "StallSimulation", currentTrueFrame );
	}

	// Tick the current true frame
	sAppStateManager.Tick( time::FrameClock::now(), time::kSimulationFrameDuration );
	sAppStateManager.ApplyDeferredStateChange();

	// Lock read heads to after the last commit, and write heads to the start
	//  of the current frame
	input::HardcodedAdvance(
		preFrameCommitRange.second + time::kSimulationFrameDuration,
		time::FrameClock::now() );

	// Commit all frames that are ready
	rollback::InclusiveTimeRange const postFrameCommitRange = rollMan.GetFramesReadyToCommit();
	rollMan.CommitFrames( postFrameCommitRange, postFrameCommitRange.second + time::kSimulationFrameDuration );

	// Lock read heads to after the last commit, and write heads to the end
	//  of the current frame
	input::HardcodedAdvance(
		postFrameCommitRange.second,
		time::FrameClock::now() );

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

	if( simulationMode == SimulationMode::StallSimulation )
	{
		// Clear out this frame so we don't re-process input from it, but can
		//  still build up additional inputs while we're stalling
		// NOTE: Non-rollback-aware code will just process the input as though
		//  it was a new frame
		controllerManager.TruncateAllRegisteredGameControllers( time::CommonClock::time_point(), previousTrueFrame );

		// Rewind time to the last frame
		time::FrameClock::set_time( previousTrueFrame );
		rollMan.SetHeadClock( previousTrueFrame );

		// HACK: Reload the state from before this frame, so that we'll repeat
		//  it again from the same time
		// TODO: Instead, we should just truncate any state from this frame
		//  here, since that should theoretically be a much cheaper option
		rollMan.LoadManualSnapshot( "StallSimulation" );
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
