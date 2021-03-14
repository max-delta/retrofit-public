#include "stdafx.h"
#include "RollbackInputManager.h"

#include "GameSync/RollbackController.h"
#include "GameSync/RollbackFilters.h"

#include "Rollback/RollbackManager.h"
#include "Rollback/InputStreamRef.h"
#include "Timing/FrameClock.h"



namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

RollbackInputManager::RollbackInputManager( WeakPtr<rollback::RollbackManager> rollMan )
	: mRollMan( rollMan )
{
	//
}



void RollbackInputManager::AddController(
	rollback::InputStreamIdentifier const& identifier,
	WeakPtr<Controller> controller )
{
	RF_ASSERT( controller != nullptr );

	WriterLock const lock( mControllersMutex );

#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	for( WeakPtr<Controller> const& curController : mControllers )
	{
		RF_ASSERT( curController != controller );
	}
#endif

	controller->SetRollbackManager( mRollMan );
	controller->SetRollbackIdentifier( identifier );
	mRollMan->CreateNewStream( identifier, time::FrameClock::now() );
	mControllers.emplace_back( controller );
}



void RollbackInputManager::ClearAllControllers()
{
	WriterLock const lock( mControllersMutex );

	mControllers.clear();
}



void RollbackInputManager::AddLocalStreams( StreamIdentifiers const& identifiers )
{
	WriterLock const lock( mLocalStreamsMutex );

	for( rollback::InputStreamIdentifier const& identifier : identifiers )
	{
		mLocalStreams.emplace( identifier );
	}
}



void RollbackInputManager::RemoveLocalStreams( StreamIdentifiers const& identifiers )
{
	WriterLock const lock( mLocalStreamsMutex );

	for( rollback::InputStreamIdentifier const& identifier : identifiers )
	{
		mLocalStreams.erase( identifier );
	}
}



RollbackInputManager::StreamIdentifiers RollbackInputManager::GetLocalStreams() const
{
	ReaderLock const lock( mLocalStreamsMutex );

	return mLocalStreams;
}



void RollbackInputManager::SubmitNonControllerInputs()
{
	WriterLock const lock( mDebugInputMutex );

	rollback::RollbackManager& rollMan = *mRollMan;
	for( rftl::pair<rollback::InputStreamIdentifier, rollback::InputEvent> const& input : mDebugQueuedTestInput )
	{
		rollback::InputStreamRef const stream = sync::RollbackFilters::GetMutableStreamRef( rollMan, input.first );
		bool const valid = sync::RollbackFilters::TryPrepareRemoteFrame( rollMan, stream, input.second.mTime );
		if( valid && input.second.mValue != rollback::kInvalidInputValue )
		{
			stream.mUncommitted.emplace_back( input.second );
		}
	}
	mDebugQueuedTestInput.clear();
}



void RollbackInputManager::TickLocalControllers()
{
	WriterLock const controllerLock( mControllersMutex );
	ReaderLock const streamsLock( mLocalStreamsMutex );

	for( WeakPtr<Controller> const& controller : mControllers )
	{
		RF_ASSERT( controller != nullptr );
		rollback::InputStreamIdentifier const identifier = controller->GetRollbackIdentifier();
		if( mLocalStreams.count( identifier ) > 0 )
		{
			controller->ProcessInput( time::FrameClock::now(), time::FrameClock::now() );
		}
	}
}



void RollbackInputManager::AdvanceControllers(
	time::CommonClock::time_point lockedFrame,
	time::CommonClock::time_point newWriteHead )
{
	WriterLock const lock( mControllersMutex );

	for( WeakPtr<Controller> const& controller : mControllers )
	{
		RF_ASSERT( controller != nullptr );
		controller->AdvanceInputStream( lockedFrame, newWriteHead );
	}
}



void RollbackInputManager::DebugQueueTestInput(
	time::CommonClock::time_point frame,
	rollback::InputStreamIdentifier streamID,
	rollback::InputValue input )
{
	WriterLock const lock( mDebugInputMutex );

	mDebugQueuedTestInput.emplace_back( streamID, rollback::InputEvent( frame, input ) );
}




void RollbackInputManager::DebugClearTestInput()
{
	WriterLock const lock( mDebugInputMutex );

	mDebugQueuedTestInput.clear();
}

///////////////////////////////////////////////////////////////////////////////
}
