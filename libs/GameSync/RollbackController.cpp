#include "stdafx.h"
#include "RollbackController.h"

#include "Rollback/RollbackManager.h"

#include "Logging/Logging.h"

#include "core_math/math_clamps.h"
#include "core_math/math_casts.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

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
	return mIdentifier;
}



void RollbackController::SetRollbackIdentifier( rollback::InputStreamIdentifier const& identifier )
{
	mIdentifier = identifier;
}



void RollbackController::ProcessInput()
{
	RF_ASSERT( mManager != nullptr );
	rollback::InputStream& inputStream = mManager->GetMutableUncommittedStreams().at( mIdentifier );

	auto const onElement = [&inputStream]( GameCommand const& element ) -> void {
		RFLOG_TEST_AND_FATAL( inputStream.back().mTime <= element.mTime, nullptr, RFCAT_GAMESYNC, "Stale input from game controller" );
		inputStream.emplace_back( element.mTime, element.mType );
	};
	rftl::virtual_callable_iterator<GameCommand, decltype( onElement )> converter( onElement );
	mSource->GetGameCommandStream( converter );
}



void RollbackController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const
{
	RF_ASSERT( mManager != nullptr );
	rollback::InputStream const& committedInputStream = mManager->GetCommittedStreams().at( mIdentifier );
	rollback::InputStream const& uncommittedInputStream = mManager->GetMutableUncommittedStreams().at( mIdentifier );

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
		rollback::InputStream::const_iterator const last = committedInputStream.end() - 1;
		rollback::InputStream::const_iterator const start =
			last - math::integer_cast<rollback::InputStream::difference_type>( commandsToPullFromCommitted );
		for( rollback::InputStream::const_iterator iter = start; start < committedInputStream.end(); iter++ )
		{
			parser( GameCommand{ iter->mValue, iter->mTime } );
			numCommandsEmmitted++;
		}
	}
	RF_ASSERT( numCommandsEmmitted == commandsToPullFromCommitted );

	// Pull from uncommitted stream
	if( commandsToPullFromCommitted > 0 )
	{
		rollback::InputStream::const_iterator const last = uncommittedInputStream.end() - 1;
		rollback::InputStream::const_iterator const start =
			last - math::integer_cast<rollback::InputStream::difference_type>( commandsToPullFromUncommitted );
		for( rollback::InputStream::const_iterator iter = start; start < uncommittedInputStream.end(); iter++ )
		{
			parser( GameCommand{ iter->mValue, iter->mTime } );
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
}}
