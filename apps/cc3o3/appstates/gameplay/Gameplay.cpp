#include "stdafx.h"
#include "Gameplay.h"

#include "cc3o3/appstates/gameplay/Gameplay_Overworld.h"
#include "cc3o3/appstates/gameplay/Gameplay_Site.h"
#include "cc3o3/appstates/gameplay/Gameplay_Battle.h"
#include "cc3o3/appstates/gameplay/Gameplay_Cutscene.h"
#include "cc3o3/appstates/gameplay/Gameplay_Menus.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/save/SaveManager.h"
#include "cc3o3/state/objects/LocalUI.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"
#include "GameAppState/AppStateTickContext.h"
#include "GameSync/SnapshotSerializer.h"

#include "Rollback/RollbackManager.h"
#include "Timing/FrameClock.h"
#include "PlatformFilesystem/VFS.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	AppStateManager mAppStateManager;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	// Setup for single-player
	RF_TODO_ANNOTATION( "Multiplayer considerations and logic, instead of single-player assumptions" );
	InputHelpers::SetSinglePlayer( input::player::P1 );
	input::HardcodedPlayerSetup( InputHelpers::GetSinglePlayer() );
	InputHelpers::MakeLocal( InputHelpers::GetSinglePlayer() );

	// Set up generic objects not controlled by campaign
	{
		using namespace state;
		using namespace state::obj;

		rollback::RollbackManager& rollMan = *gRollbackManager;
		rollback::Domain& sharedDomain = rollMan.GetMutableSharedDomain();
		rollback::Domain& privateDomain = rollMan.GetMutablePrivateDomain();
		rollback::Window& sharedWindow = sharedDomain.GetMutableWindow();
		rollback::Window& privateWindow = privateDomain.GetMutableWindow();

		// Set up the UI
		{
			VariableIdentifier const localUIRoot( "localUI" );
			CreateLocalUI( sharedWindow, privateWindow, localUIRoot );
		}
	}

	// HACK: Choose arbitrary save
	RF_TODO_ANNOTATION( "Actual save management logic should've happened before this" );
	save::SaveManager::SaveName saveName;
	{
		save::SaveManager const& saveMan = *gSaveManager;
		save::SaveManager::SaveNames const saveNames = saveMan.FindSaveNames();
		RF_ASSERT( saveNames.size() == 1 );
		saveName = *saveNames.begin();
	}

	// Load save
	{
		bool const loaded = gSaveManager->PerformInitialLoad( saveName );
		RFLOG_TEST_AND_FATAL( loaded, nullptr, RFCAT_CC3O3, "Failed to load save data" );
	}

	// HACK: Save out the state for diagnostics
	RF_TODO_ANNOTATION( "More formal snapshotting" );
	{
		rollback::RollbackManager& rollMan = *gRollbackManager;
		rollback::Domain& sharedDomain = rollMan.GetMutableSharedDomain();
		rollback::Domain& privateDomain = rollMan.GetMutablePrivateDomain();

		WeakPtr<rollback::Snapshot const> const sharedSnapshot =
			sharedDomain.TakeManualSnapshot( "GameStart", time::FrameClock::now() );
		RF_ASSERT( sharedSnapshot != nullptr );
		WeakPtr<rollback::Snapshot const> const privateSnapshot =
			privateDomain.TakeManualSnapshot( "GameStart", time::FrameClock::now() );
		RF_ASSERT( privateSnapshot != nullptr );

		file::VFS const& vfs = *app::gVfs;
		sync::SnapshotSerializer::SerializeToDiagnosticFile(
			*sharedSnapshot, vfs, file::VFS::kRoot.GetChild( "scratch", "snapshots", "sharedTest.diag" ) );
		sync::SnapshotSerializer::SerializeToDiagnosticFile(
			*privateSnapshot, vfs, file::VFS::kRoot.GetChild( "scratch", "snapshots", "privateTest.diag" ) );
	}

	// Start sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.AddState( appstate::id::Gameplay_Overworld, DefaultCreator<appstate::Gameplay_Overworld>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Site, DefaultCreator<appstate::Gameplay_Site>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Battle, DefaultCreator<appstate::Gameplay_Battle>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Cutscene, DefaultCreator<appstate::Gameplay_Cutscene>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Menus, DefaultCreator<appstate::Gameplay_Menus>::Create() );
	appStateMan.Start( appstate::GetFirstGameplayState() );
}



void Gameplay::OnExit( AppStateChangeContext& context )
{
	// Stop sub-states
	InternalState& internalState = *mInternalState;
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.Stop();

	mInternalState = nullptr;

	RF_TODO_ANNOTATION( "Multiplayer considerations and logic, instead of single-player assumptions" );
	InputHelpers::ClearSinglePlayer();
}



void Gameplay::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;

	// Tick sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.Tick( context.mCurrentTime, context.mElapsedTimeSinceLastTick );
	appStateMan.ApplyDeferredStateChange();
}

///////////////////////////////////////////////////////////////////////////////
}
