#include "stdafx.h"
#include "cc3o3.h"

#include "cc3o3/appstates/Boot.h"
#include "cc3o3/appstates/InitialLoading.h"
#include "cc3o3/appstates/titlescreen/TitleScreen.h"
#include "cc3o3/appstates/gameplay/Gameplay.h"
#include "cc3o3/appstates/devtest/DevTestCombatCharts.h"
#include "cc3o3/appstates/devtest/DevTestGridCharts.h"
#include "cc3o3/appstates/devtest/DevTestLobby.h"
#include "cc3o3/appstates/devtest/DevTestRollback.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/DeveloperHud.h"
#include "cc3o3/input/InputFwd.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/time/TimeFwd.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"
#include "GameInput/ControllerManager.h"
#include "GameSync/RollbackInputManager.h"
#include "GameUI/ContainerManager.h"

#include "PPU/PPUController.h"
#include "Timing/FrameClock.h"
#include "Rollback/RollbackManager.h"
#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"

#include "rftl/optional"


namespace RF::cc {
///////////////////////////////////////////////////////////////////////////////

constexpr bool kAllowDebug = true;

constexpr bool kDebugGrid = kAllowDebug && true;
constexpr bool kDebugUI = kAllowDebug && true;
constexpr bool kDebugUIScaler = kAllowDebug && false;
constexpr bool kDebugHideZoomFactor = kAllowDebug && false;
constexpr bool kDebugSuppressRender = kAllowDebug && false;
constexpr bool kDebugUILayering = kAllowDebug && true;
constexpr bool kDebugUIAnchors = kAllowDebug && false;
constexpr bool kDebugUIHidden = kAllowDebug && false;

constexpr bool kAllowDevTests = true;
constexpr bool kAllowDeveloperHud = true;

static appstate::AppStateManager sAppStateManager;

static SimulationMode sDebugPreviousFrameSimulationMode = SimulationMode::Invalid;

static rftl::optional<rftl::string> sSnapshotTakeQueued = {};
static rftl::optional<rftl::string> sSnapshotLoadQueued = {};

static bool sInstantReplayTriggered = false;
static time::CommonClock::time_point sInstantReplayEndTime = {};

static rftl::optional<time::CommonClock::time_point> sHardTimeReset = {};


// This is important enough that we want it to show up on the callstack, but
//  want to keep all the complex rollback logic together
template<typename FuncT>
RF_NO_INLINE void RollbackTickWrapper( FuncT const& func )
{
	func();
}

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	SystemStartup();

	sAppStateManager.AddState( appstate::id::Boot, DefaultCreator<appstate::Boot>::Create() );
	sAppStateManager.AddState( appstate::id::InitialLoading, DefaultCreator<appstate::InitialLoading>::Create() );
	sAppStateManager.AddState( appstate::id::TitleScreen, DefaultCreator<appstate::TitleScreen>::Create() );
	sAppStateManager.AddState( appstate::id::Gameplay, DefaultCreator<appstate::Gameplay>::Create() );
	if( kAllowDevTests )
	{
		sAppStateManager.AddState( appstate::id::DevTestCombatCharts, DefaultCreator<appstate::DevTestCombatCharts>::Create() );
		sAppStateManager.AddState( appstate::id::DevTestGridCharts, DefaultCreator<appstate::DevTestGridCharts>::Create() );
		sAppStateManager.AddState( appstate::id::DevTestLobby, DefaultCreator<appstate::DevTestLobby>::Create() );
		sAppStateManager.AddState( appstate::id::DevTestRollback, DefaultCreator<appstate::DevTestRollback>::Create() );
	}

	sAppStateManager.Start( appstate::GetBootState() );

	app::gGraphics->DebugSetGridEnabled( kDebugGrid );
	app::gGraphics->HideZoomFactor( kDebugHideZoomFactor );

	// Keep the pixels flush with the edges of the screen, to make some more
	//  precise UI math easier
	app::gUiManager->SetRootAABBReduction( 0 );

	if( kDebugUIScaler )
	{
		app::gUiManager->SetDebugAABBReduction( 2 );
	}

	if( kAllowDeveloperHud )
	{
		developer::Startup();
	}

	// Mark our boot in the user directory, mostly to make sure it can be
	//  written to in VFS
	file::VFSPath const bootStamp = file::VFS::kRoot.GetChild( "user", "boot.stamp" );
	bool const stampSuccess = app::gVfs->GetFileForWrite( bootStamp ) != nullptr;
	RFLOG_TEST_AND_FATAL( stampSuccess, bootStamp, RFCAT_CC3O3, "Failed to mark boot stamp" );
}



void ProcessFrame()
{
	rollback::RollbackManager& rollMan = *gRollbackManager;
	input::ControllerManager& controllerManager = *app::gInputControllerManager;

	auto const isInstantReplayInProgress = [&rollMan]() -> bool //
	{
		return rollMan.GetHeadClock() <= sInstantReplayEndTime;
	};

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

	if( isInstantReplayInProgress() == false )
	{
		// Clear the replay so a save/load state can't re-trigger it
		sInstantReplayEndTime = {};
	}

	if( kAllowDeveloperHud )
	{
		// Developer input is global, and processed seperately from app states
		developer::ProcessInput();
	}

	// Check for things that could trigger a time change
	{
		auto const triggerTimeChange = [&rollMan]( time::CommonClock::time_point time ) -> void //
		{
			// NOTE: Setting frame clock to max, so all time changes are
			//  interpreted as a rewind
			time::FrameClock::set_time( time::CommonClock::kMax );
			RF_ASSERT( time < time::CommonClock::kMax );
			rollMan.SetHeadClock( time );
		};

		if( sSnapshotTakeQueued.has_value() )
		{
			rollMan.TakeManualSnapshot( sSnapshotTakeQueued.value(), time::FrameClock::now() );
			sSnapshotTakeQueued.reset();
		}

		if( sSnapshotLoadQueued.has_value() )
		{
			time::CommonClock::time_point const time = rollMan.LoadManualSnapshot( sSnapshotLoadQueued.value() );
			triggerTimeChange( time );
			sSnapshotLoadQueued.reset();
		}

		if( sHardTimeReset.has_value() )
		{
			triggerTimeChange( sHardTimeReset.value() );
			sHardTimeReset.reset();
		}
	}

	if( rollMan.GetHeadClock() < time::FrameClock::now() )
	{
		// The head has moved backwards in time since we started the frame
		if( sInstantReplayTriggered )
		{
			sInstantReplayTriggered = false;

			// This was caused by a replay, so we need to stop processing any
			//  true input while we wait for the replay to catch up to the
			//  current true time
			RFLOG_INFO( nullptr, RFCAT_CC3O3, "Rollback detected for instant replay" );

			// The head clock is presumed to be valid, so we should begin
			//  processing the frame afterwards (which is now what we're
			//  considering the current frame)
			time::CommonClock::time_point const frameAfterHead = rollMan.GetHeadClock() + time::kSimulationFrameDuration;
			time::FrameClock::set_time( frameAfterHead );
			rollMan.SetHeadClock( frameAfterHead );
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
			gRollbackInputManager->DebugClearTestInput();

			// The head clock is presumed to be valid, so we should begin
			//  processing the frame afterwards (which is now what we're
			//  considering the current frame)
			time::CommonClock::time_point const frameAfterHead = rollMan.GetHeadClock() + time::kSimulationFrameDuration;
			time::FrameClock::set_time( frameAfterHead );
			rollMan.SetHeadClock( frameAfterHead );
		}
	}

	// Submit queued up input from debug as well as more legitimate systems
	//  like networking
	gRollbackInputManager->SubmitNonControllerInputs();

	// We're about to do some really weird stuff to time, so we need to make
	//  sure we don't lose track of what frame we're ultimately trying to
	//  put together
	time::CommonClock::time_point const currentTrueFrame = time::FrameClock::now();
	time::CommonClock::time_point const previousTrueFrame = currentTrueFrame - time::kSimulationFrameDuration;
	RF_ASSERT( previousTrueFrame < currentTrueFrame );

	// Figure out what type of simulation we need to perform
	SimulationMode simulationMode = SimulationMode::Invalid;
	time::CommonClock::time_point const preFrameCommitHead = rollMan.GetMaxCommitHead();
	rollback::InclusiveTimeRange preFrameCommitRange = { preFrameCommitHead, preFrameCommitHead };
	if( isInstantReplayInProgress() )
	{
		// Instant replay active, simulating frames with whatever input they
		//  already had and blocking any new input
		simulationMode = SimulationMode::OnRailsReplay;
	}
	else if( preFrameCommitHead >= currentTrueFrame )
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
	RF_TODO_ANNOTATION( "Have a manifest for what is locally-owned" );
	(void)( &sync::RollbackInputManager::AdvanceControllers );

	// Commit all frames that are ready
	rollMan.CommitFrames( preFrameCommitRange, preFrameCommitRange.second );

	if( simulationMode == SimulationMode::OnRailsReplay )
	{
		// Input disabled during replay
	}
	else
	{
		// Lock read heads to after the last commit, and write heads to the
		//  start of the current frame
		gRollbackInputManager->AdvanceControllers(
			preFrameCommitRange.second + time::kSimulationFrameDuration,
			time::FrameClock::now() );
	}

	if( simulationMode == SimulationMode::RollbackAndSimulate )
	{
		// Rollback and simulate each frame up to the current true frame

		gfx::PPUController& ppu = *app::gGraphics;

		// Suppress any draw requests while performing rollback re-simulation
		ppu.SuppressDrawRequests( true );

		// Rewind to right before the commit range, since logic differences
		//  mean that some variables might not get overwritten naturally
		rollMan.RewindAllDomains( preFrameCommitRange.first - time::CommonClock::duration( 1 ) );

		// Tick every frame up to the current frame
		// NOTE: We are possibly ticking frames past the commit range, in which
		//  case we will need to do another rollback later when those frames
		//  can be committed
		time::CommonClock::time_point resimulationTime = preFrameCommitRange.first;
		RF_ASSERT( resimulationTime < currentTrueFrame );
		while( resimulationTime < currentTrueFrame )
		{
			// Adjust the time
			time::FrameClock::set_time( resimulationTime );

			// Tick the frame
			RollbackTickWrapper( []() -> void {
				sAppStateManager.Tick( time::FrameClock::now(), time::kSimulationFrameDuration );

				// WARNING: Intentionally not re-simulating global state change
				// NOTE: Behaviour here will be erratic, but likely non-fatal
				//sAppStateManager.ApplyDeferredStateChange();
			} );

			// Move to the next frame
			resimulationTime += time::kSimulationFrameDuration;
		}
		RF_ASSERT( resimulationTime == currentTrueFrame );

		// Restore the time
		time::FrameClock::set_time( currentTrueFrame );

		// Throw away any test input that debug code may have just produced
		gRollbackInputManager->DebugClearTestInput();

		// Restore graphics before ticking true frame
		ppu.SuppressDrawRequests( false );
	}


	if( simulationMode == SimulationMode::OnRailsReplay )
	{
		// Input disabled during replay
	}
	else
	{
		gRollbackInputManager->TickLocalControllers();
	}

	// Tick the current true frame
	sAppStateManager.Tick( time::FrameClock::now(), time::kSimulationFrameDuration );

	// WARNING: Rollback behavior unreliable across global state changes
	sAppStateManager.ApplyDeferredStateChange();

	if( simulationMode == SimulationMode::OnRailsReplay )
	{
		// Input disabled during replay
	}
	else
	{
		// Lock read heads to after the last commit, and write heads to the start
		//  of the current frame
		gRollbackInputManager->AdvanceControllers(
			preFrameCommitRange.second + time::kSimulationFrameDuration,
			time::FrameClock::now() );
	}

	if( rollMan.HasInputStreams() == false )
	{
		// No input streams, simulation is running freely
	}
	else
	{
		// Commit all frames that are ready
		rollback::InclusiveTimeRange const postFrameCommitRange = rollMan.GetFramesReadyToCommit();
		rollMan.CommitFrames( postFrameCommitRange, postFrameCommitRange.second + time::kSimulationFrameDuration );

		if( simulationMode == SimulationMode::OnRailsReplay )
		{
			// Input disabled during replay
		}
		else
		{
			// Lock read heads to after the last commit, and write heads to the end
			//  of the current frame
			gRollbackInputManager->AdvanceControllers(
				postFrameCommitRange.second,
				time::FrameClock::now() );
		}
	}

	ui::ContainerManager& uiMan = *app::gUiManager;
	uiMan.RecalcRootContainer();
	uiMan.ProcessRecalcs();
	if( kDebugUI )
	{
		uiMan.DebugRender( kDebugSuppressRender || kDebugUILayering, kDebugUIAnchors, kDebugUIHidden );
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

		// Rewind the state to before this frame, so that we'll repeat it again
		//  from the same time when we repeat this frame
		rollMan.RewindAllDomains( previousTrueFrame );
	}

	// Note what we did this frame, for diagnostics
	sDebugPreviousFrameSimulationMode = simulationMode;
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



SimulationMode DebugGetPreviousFrameSimulationMode()
{
	return sDebugPreviousFrameSimulationMode;
}



void DebugTakeSnapshot( rftl::string const& name )
{
	sSnapshotTakeQueued = name;
	RFLOG_INFO( nullptr, RFCAT_CC3O3, "Debug take snapshot triggered" );
}



void DebugLoadSnapshot( rftl::string const& name )
{
	sSnapshotLoadQueued = name;
	RFLOG_INFO( nullptr, RFCAT_CC3O3, "Debug load snapshot triggered" );
}



void DebugInstantReplay( size_t numFrames )
{
	rollback::RollbackManager& rollMan = *gRollbackManager;

	if( rollMan.GetHeadClock() <= sInstantReplayEndTime )
	{
		// Currently in a replay
		RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Instant replay request ignored" );
		return;
	}

	sInstantReplayEndTime = time::FrameClock::now();
	rollMan.SetHeadClock( time::FrameClock::now() - time::kSimulationFrameDuration * numFrames );
	sInstantReplayTriggered = true;
	RFLOG_INFO( nullptr, RFCAT_CC3O3, "Instant replay triggered for frames %llu -> %llu",
		rollMan.GetHeadClock().time_since_epoch().count(),
		sInstantReplayEndTime.time_since_epoch().count() );
}



void DebugHardTimeReset()
{
	sHardTimeReset = time::CommonClock::TimePointFromNanos( 0 );
	RFLOG_INFO( nullptr, RFCAT_CC3O3, "Hard time reset triggered" );
}



void RequestGlobalDeferredStateChange( appstate::AppStateID stateID )
{
	sAppStateManager.RequestDeferredStateChange( stateID );
}

///////////////////////////////////////////////////////////////////////////////
}
