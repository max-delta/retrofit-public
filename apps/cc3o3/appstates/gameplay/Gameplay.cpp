#include "stdafx.h"
#include "Gameplay.h"

#include "cc3o3/appstates/gameplay/Gameplay_Overworld.h"
#include "cc3o3/appstates/gameplay/Gameplay_Site.h"
#include "cc3o3/appstates/gameplay/Gameplay_Battle.h"
#include "cc3o3/appstates/gameplay/Gameplay_Menus.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/Common.h"
#include "cc3o3/char/CharacterDatabase.h"

#include "GameAppState/AppStateManager.h"
#include "GameAppState/AppStateTickContext.h"

#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
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

	// TODO: Load save

	// Load characters
	bool const charLoad = gCharacterDatabase->LoadFromPersistentStorage();
	RFLOG_TEST_AND_FATAL( charLoad, nullptr, RFCAT_CC3O3, "Failed to load character database" );

	// Start sub-states
	AppStateManager& appStateMan = internalState.mAppStateManager;
	appStateMan.AddState( appstate::id::Gameplay_Overworld, DefaultCreator<appstate::Gameplay_Overworld>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Site, DefaultCreator<appstate::Gameplay_Site>::Create() );
	appStateMan.AddState( appstate::id::Gameplay_Battle, DefaultCreator<appstate::Gameplay_Battle>::Create() );
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
}}}
