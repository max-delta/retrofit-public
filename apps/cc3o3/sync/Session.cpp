#include "stdafx.h"
#include "Session.h"

#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/cc3o3.h" // HACK: Last frame simulation mode
#include "cc3o3/Common.h"

#include "GameSync/SessionClientManager.h"
#include "GameSync/SessionHostManager.h"
#include "GameSync/RollbackInputManager.h"


namespace RF::cc::sync {
///////////////////////////////////////////////////////////////////////////////

// Session manager storage / access
WeakPtr<sync::SessionManager> gSessionManager;
WeakPtr<sync::SessionHostManager> gSessionHostManager;
WeakPtr<sync::SessionClientManager> gSessionClientManager;
static UniquePtr<sync::SessionHostManager> sSessionHostManager;
static UniquePtr<sync::SessionClientManager> sSessionClientManager;

///////////////////////////////////////////////////////////////////////////////

void SetSessionHostManager( UniquePtr<sync::SessionHostManager>&& manager )
{
	RF_ASSERT( gSessionManager == sSessionHostManager || gSessionManager == sSessionClientManager );
	RF_ASSERT( gSessionHostManager == sSessionHostManager );
	RF_ASSERT( gSessionClientManager == sSessionClientManager );
	RF_ASSERT( sSessionHostManager == nullptr );
	RF_ASSERT( sSessionClientManager == nullptr );
	sSessionHostManager = rftl::move( manager );
	gSessionHostManager = sSessionHostManager;
	gSessionManager = sSessionHostManager;
}



void SetSessionClientManager( UniquePtr<sync::SessionClientManager>&& manager )
{
	RF_ASSERT( gSessionManager == sSessionHostManager || gSessionManager == sSessionClientManager );
	RF_ASSERT( gSessionHostManager == sSessionHostManager );
	RF_ASSERT( gSessionClientManager == sSessionClientManager );
	RF_ASSERT( sSessionHostManager == nullptr );
	RF_ASSERT( sSessionClientManager == nullptr );
	sSessionClientManager = rftl::move( manager );
	gSessionClientManager = sSessionClientManager;
	gSessionManager = sSessionClientManager;
}



void ClearSessionManager()
{
	RF_ASSERT( gSessionManager == sSessionHostManager || gSessionManager == sSessionClientManager );
	RF_ASSERT( gSessionHostManager == sSessionHostManager );
	RF_ASSERT( gSessionClientManager == sSessionClientManager );
	RF_ASSERT( sSessionHostManager != nullptr || sSessionClientManager != nullptr );
	gSessionManager = nullptr;
	gSessionHostManager = nullptr;
	gSessionClientManager = nullptr;
	sSessionHostManager = nullptr;
	sSessionClientManager = nullptr;
}

///////////////////////////////////////////////////////////////////////////////

bool QueueRollbackInputForSend( time::CommonClock::time_point frameReadyToCommit )
{
	if( gSessionManager == nullptr )
	{
		return false;
	}

	return gSessionManager->QueueOutgoingRollbackInputPack(
		gRollbackInputManager->PreparePackForSend(
			frameReadyToCommit ) );
}



void ProcessSessionNetworkOperations()
{
	if( sync::gSessionHostManager != nullptr )
	{
		SessionHostManager& host = *sync::gSessionHostManager;
		host.ProcessPendingOperations();
	}
	if( sync::gSessionClientManager != nullptr )
	{
		SessionClientManager& client = *sync::gSessionClientManager;
		client.ProcessPendingOperations();
	}
}



void ProcessSessionControllerOperations()
{
	if( sync::gSessionManager == nullptr )
	{
		return;
	}
	SessionManager& manager = *sync::gSessionManager;

	SessionMembers const members = manager.GetSessionMembers();

	// Update controller modes
	{
		using PlayerIDs = SessionMembers::PlayerIDs;
		PlayerIDs const all = members.GetPlayerIDs();
		PlayerIDs const local = members.GetLocalPlayerIDs();
		for( input::PlayerID const& id : all )
		{
			bool const isLocal = local.count( id ) > 0;
			if( isLocal )
			{
				appstate::InputHelpers::MakeLocal( id );
			}
			else
			{
				appstate::InputHelpers::MakeRemote( id );
			}
		}
	}

	// Update remote input
	{
		sync::RollbackInputManager& rollInputMan = *gRollbackInputManager;

		SessionManager::RollbackSourcedPacks packs = manager.ConsumeRollbackInputPacks();
		for( SessionManager::RollbackSourcedPack& sourcedPack : packs )
		{
			RF_TODO_ANNOTATION( "Filter out input streams that don't appear to belong to the connection" );
			rollInputMan.ApplyPackFromRemote( rftl::move( sourcedPack.mInputPack ) );
		}
	};
}

///////////////////////////////////////////////////////////////////////////////

RecommendedFrameSpeed CalcRecommendedFrameSpeed()
{
	if( appstate::InputHelpers::HasRemotePlayers() == false )
	{
		// No remote players, maintain speed
		return RecommendedFrameSpeed::Maintain;
	}

	// HACK: Adjust based on previous rollback behavior
	RF_TODO_ANNOTATION( "More intelligent tuning, based on observed input stream read/write heads" );
	SimulationMode const previousMode = DebugGetPreviousFrameSimulationMode();
	if( previousMode == SimulationMode::RollbackAndSimulate )
	{
		return RecommendedFrameSpeed::SlowDown;
	}
	if( previousMode == SimulationMode::SingleFrameSimulate )
	{
		return RecommendedFrameSpeed::SpeedUp;
	}

	return RecommendedFrameSpeed::Maintain;
}

///////////////////////////////////////////////////////////////////////////////
}
