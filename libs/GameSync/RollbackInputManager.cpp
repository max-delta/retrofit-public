#include "stdafx.h"
#include "RollbackInputManager.h"

#include "GameSync/RollbackController.h"
#include "GameSync/PassthroughController.h"
#include "GameSync/RollbackFilters.h"

#include "Rollback/RollbackManager.h"
#include "Rollback/InputStreamRef.h"
#include "Timing/FrameClock.h"
#include "Logging/Logging.h"

#include "core_math/math_casts.h"


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



void RollbackInputManager::AddPassthrough( WeakPtr<Passthrough> passthrough )
{
	RF_ASSERT( passthrough != nullptr );

	WriterLock const lock( mPassthroughsMutex );

#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	for( WeakPtr<Passthrough> const& curPassthrough : mPassthroughs )
	{
		RF_ASSERT( curPassthrough != passthrough );
	}
#endif

	mPassthroughs.emplace_back( passthrough );
}



void RollbackInputManager::ClearAllPassthroughs()
{
	WriterLock const lock( mPassthroughsMutex );

	mPassthroughs.clear();
}



void RollbackInputManager::SuppressAllPassthroughs( bool suppress )
{
	WriterLock const lock( mPassthroughsMutex );

	for( WeakPtr<Passthrough> const& passthrough : mPassthroughs )
	{
		RF_ASSERT( passthrough != nullptr );
		passthrough->SuppressCommands( suppress );
	}
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



void RollbackInputManager::AdvanceLocalControllers(
	time::CommonClock::time_point lockedFrame,
	time::CommonClock::time_point newWriteHead )
{
	WriterLock const lock( mControllersMutex );

	for( WeakPtr<Controller> const& controller : mControllers )
	{
		RF_ASSERT( controller != nullptr );
		rollback::InputStreamIdentifier const identifier = controller->GetRollbackIdentifier();
		if( mLocalStreams.count( identifier ) > 0 )
		{
			controller->AdvanceInputStream( lockedFrame, newWriteHead );
		}
	}
}



RollbackInputPack RollbackInputManager::PreparePackForSend(
	time::CommonClock::time_point frameReadyToCommit ) const
{
	RollbackInputPack retVal = {};
	retVal.mFrameReadyToCommit = frameReadyToCommit;

	StreamIdentifiers const localStreams = GetLocalStreams();

	// Enumerate all uncommitted streams
	// NOTE: A committed stream would already be committed on all peers as
	//  well, so is not useful to send
	using InputStreams = rollback::RollbackManager::InputStreams;
	InputStreams const& streams = mRollMan->GetUncommittedStreams();
	for( InputStreams::value_type const& streamEntry : streams )
	{
		rollback::InputStreamIdentifier const& identifier = streamEntry.first;
		rollback::InputStream const& stream = streamEntry.second;

		if( localStreams.count( identifier ) <= 0 )
		{
			// Not one of ours, ignore
			continue;
		}

		// Add all the events
		RollbackInputPack::Events& eventsToSend = retVal.mStreams[identifier];
		for( rollback::InputEvent const& event : stream )
		{
			// Streams use an invalid event to denote the read head, ignore it
			if( event.mValue != rollback::kInvalidInputValue )
			{
				eventsToSend.emplace_back( event );
			}
		}
	}

	return retVal;
}



void RollbackInputManager::ApplyPackFromRemote( RollbackInputPack&& pack )
{
	rollback::RollbackManager& rollMan = *mRollMan;

	// For each stream...
	for( RollbackInputPack::Streams::value_type const& streamEntry : pack.mStreams )
	{
		rollback::InputStreamIdentifier const& identifier = streamEntry.first;
		RollbackInputPack::Events const& events = streamEntry.second;
		rollback::InputStreamRef const stream = sync::RollbackFilters::GetMutableStreamRef( rollMan, identifier );

		if( events.empty() == false )
		{
			size_t overlapCount;
			bool const canOverlap = sync::RollbackFilters::CanOverlapOrSuffix( rollMan, stream, events, overlapCount );
			if( canOverlap == false )
			{
				RFLOG_ERROR( nullptr, RFCAT_GAMESYNC, "No overlap found on rollback merge" );
				RF_TODO_BREAK_MSG(
					"Figure out the best policy for race conditions on"
					" controller or time changes. Multi-frame buffer in"
					" gameplay, or grace windows?" );
			}
			RF_ASSERT( overlapCount <= events.size() );
			RollbackInputPack::Events const overhangEvents(
				events.begin() + math::integer_cast<RollbackInputPack::Events::difference_type>( overlapCount ),
				events.end() );

			// For each event...
			for( rollback::InputEvent const& input : overhangEvents )
			{
				bool const valid = sync::RollbackFilters::TryPrepareRemoteFrame( rollMan, stream, input.mTime );
				if( valid && input.mValue != rollback::kInvalidInputValue )
				{
					stream.mUncommitted.emplace_back( input );
				}
			}
		}

		// Input should normally be quite sparse, see if the remote says we can
		//  commit farther than the last event
		bool tryCommitPastInput = true;
		if( events.empty() == false )
		{
			if( pack.mFrameReadyToCommit < events.back().mTime )
			{
				tryCommitPastInput = false;
			}
		}
		if( tryCommitPastInput )
		{
			bool const tentativeCommitValid = sync::RollbackFilters::TryPrepareRemoteFrame( rollMan, stream, pack.mFrameReadyToCommit );
			RF_ASSERT( tentativeCommitValid );
		}
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
