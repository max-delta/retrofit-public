#include "stdafx.h"
#include "AppStateManager.h"

#include "GameAppState/AppStateRef.h"
#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateChangeContext.h"

#include "core/ptr/default_creator.h"


namespace RF::appstate {
///////////////////////////////////////////////////////////////////////////////

AppStateManager::AppStateManager()
	: FiniteStateMachine( RF::ExplicitDefaultConstruct() )
	, mBackingStateCollection( DefaultCreator<FiniteStateMachine::StateCollection>::Create() )
{
	SetStateCollection( mBackingStateCollection );
}



void AppStateManager::AddState( AppStateID stateID, UniquePtr<AppState>&& state )
{
	RF_ASSERT( state != nullptr );
	FiniteStateMachine::StateCollection::Storage& storage = mBackingStateCollection->mStates;
	RF_ASSERT( storage.count( stateID ) == 0 );
	storage[stateID].mAppState = rftl::move( state );
}



void AppStateManager::Start( AppStateID initialStateID )
{
	FiniteStateMachine::Start( initialStateID );
}



void AppStateManager::Tick( time::CommonClock::time_point currentTime, time::CommonClock::duration elapsedTimeSinceLastTick )
{
	AppStateTickContext context{ *this };
	context.mCurrentTime = currentTime;
	context.mElapsedTimeSinceLastTick = elapsedTimeSinceLastTick;

	GetCurrentMutableState()->OnTick( context );
}



void AppStateManager::Stop()
{
	FiniteStateMachine::Stop();
}



void AppStateManager::RequestDeferredStateChange( AppStateID stateID ) const
{
	RF_ASSERT( mDeferredStateChange.has_value() == false );
	mDeferredStateChange = stateID;
}



void AppStateManager::ApplyDeferredStateChange()
{
	if( mDeferredStateChange.has_value() )
	{
		AppStateID const newStateID = *mDeferredStateChange;
		mDeferredStateChange.reset();
		ChangeState( newStateID );
	}
}

///////////////////////////////////////////////////////////////////////////////

AppStateChangeContext AppStateManager::CreateStateChangeContext()
{
	return AppStateChangeContext( *this );
}

///////////////////////////////////////////////////////////////////////////////
}
