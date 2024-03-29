#include "stdafx.h"
#include "RollbackController.h"

#include "Rollback/RollbackManager.h"

#include "Logging/Logging.h"

#include "core_math/math_clamps.h"
#include "core_math/math_casts.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

RollbackController::RollbackController()
	: GameController( "Rollback" )
{
	//
}



WeakPtr<GameController> RollbackController::GetSource() const
{
	return mSource;
}



void RollbackController::SetSource( WeakPtr<GameController> const& source )
{
	mSource = source;
}



WeakPtr<rollback::RollbackManager> RollbackController::GetRollbackManager() const
{
	return mManager;
}



void RollbackController::SetRollbackManager( WeakPtr<rollback::RollbackManager> const& manager )
{
	mManager = manager;
}



rollback::InputStreamIdentifier RollbackController::GetRollbackIdentifier() const
{
	return mStreamIdentifier;
}



void RollbackController::SetRollbackIdentifier( rollback::InputStreamIdentifier const& identifier )
{
	mStreamIdentifier = identifier;
}



time::CommonClock::duration RollbackController::GetArtificialDelay() const
{
	return mArtificalDelay;
}



void RollbackController::SetArtificialDelay( time::CommonClock::duration const& delay )
{
	RF_ASSERT( delay.count() > 0 );
	mArtificalDelay = delay;
}



void RollbackController::ProcessInput( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime )
{
	RF_ASSERT( mManager != nullptr );
	time::CommonClock::duration const artificalDelay = mArtificalDelay;

	rollback::InputStream& inputStream = mManager->GetMutableUncommittedStreams().at( mStreamIdentifier );
	RFLOG_TEST_AND_FATAL( inputStream.back().mTime <= earliestTime + artificalDelay, nullptr, RFCAT_GAMESYNC, "Input processing request can result in stale data" );

	auto const onElement = [&inputStream, &artificalDelay]( GameCommand const& element ) -> void {
		time::CommonClock::time_point const artificialTime = element.mTime + artificalDelay;
		RFLOG_TEST_AND_FATAL( inputStream.back().mTime <= artificialTime, nullptr, RFCAT_GAMESYNC, "Stale input from game controller" );
		inputStream.emplace_back( artificialTime, element.mType );
	};
	rftl::virtual_callable_iterator<GameCommand, decltype( onElement )> converter( onElement );
	mSource->GetGameCommandStream( converter, earliestTime, latestTime );
}



void RollbackController::AdvanceInputStream( time::CommonClock::time_point lockedFrame, time::CommonClock::time_point newWriteHead )
{
	RF_ASSERT( mManager != nullptr );
	rollback::InputStream& uncommittedInputStream = mManager->GetMutableUncommittedStreams().at( mStreamIdentifier );
	uncommittedInputStream.increase_write_head( newWriteHead + mArtificalDelay );
	uncommittedInputStream.increase_read_head( lockedFrame );
}



void RollbackController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const
{
	RF_ASSERT( mManager != nullptr );
	rollback::InputStream const& committedInputStream = mManager->GetCommittedStreams().at( mStreamIdentifier );
	rollback::InputStream const& uncommittedInputStream = mManager->GetMutableUncommittedStreams().at( mStreamIdentifier );

	size_t const numCommitted = committedInputStream.size();
	size_t const numUncommitted = uncommittedInputStream.size();
	size_t const commandsToPullTotal = math::Min( numCommitted + numUncommitted, maxCommands );
	size_t const commandsToPullFromCommitted =
		commandsToPullTotal > numUncommitted ?
		commandsToPullTotal - numUncommitted :
		0;
	size_t const commandsToPullFromUncommitted = commandsToPullTotal - commandsToPullFromCommitted;
	size_t numCommandsEmmitted = 0;

	// Pull from committed stream
	if( commandsToPullFromCommitted > 0 )
	{
		rollback::InputStream::const_iterator const start =
			committedInputStream.end() -
			math::integer_cast<rollback::InputStream::difference_type>( commandsToPullFromCommitted );
		for( rollback::InputStream::const_iterator iter = start; iter < committedInputStream.end(); iter++ )
		{
			if( iter->mValue != rollback::kInvalidInputValue )
			{
				parser( GameCommand{ iter->mValue, iter->mTime } );
			}
			else
			{
				// Marker for an empty frame, ignore
			}
			numCommandsEmmitted++;
		}
	}
	RF_ASSERT( numCommandsEmmitted == commandsToPullFromCommitted );

	// Pull from uncommitted stream
	if( commandsToPullFromUncommitted > 0 )
	{
		rollback::InputStream::const_iterator const start =
			uncommittedInputStream.end() -
			math::integer_cast<rollback::InputStream::difference_type>( commandsToPullFromUncommitted );
		for( rollback::InputStream::const_iterator iter = start; iter < uncommittedInputStream.end(); iter++ )
		{
			if( iter->mValue != rollback::kInvalidInputValue )
			{
				parser( GameCommand{ iter->mValue, iter->mTime } );
			}
			else
			{
				// Marker for an empty frame, ignore
			}
			numCommandsEmmitted++;
		}
	}
	RF_ASSERT( numCommandsEmmitted == commandsToPullTotal );
}



void RollbackController::TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime )
{
	mSource->TruncateBuffers( earliestTime, latestTime );
}

///////////////////////////////////////////////////////////////////////////////
}
