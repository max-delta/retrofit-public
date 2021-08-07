#include "stdafx.h"
#include "PassthroughController.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

WeakPtr<GameController> PassthroughController::GetSource() const
{
	return mSource;
}



void PassthroughController::SetSource( WeakPtr<GameController> const& source )
{
	mSource = source;
}



void PassthroughController::SuppressCommands( bool suppress )
{
	mSuppressCommands = suppress;
}



void PassthroughController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const
{
	if( mSuppressCommands )
	{
		return;
	}
	RF_ASSERT( mSource != nullptr );
	return mSource->GetGameCommandStream( parser, maxCommands );
}



void PassthroughController::GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter, size_t maxTypes ) const
{
	RF_ASSERT( mSource != nullptr );
	return mSource->GetKnownSignals( iter, maxTypes );
}



void PassthroughController::GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, size_t maxSamples, GameSignalType type ) const
{
	RF_ASSERT( mSource != nullptr );
	return mSource->GetGameSignalStream( sampler, maxSamples, type );
}



void PassthroughController::TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime )
{
	mSource->TruncateBuffers( earliestTime, latestTime );
}

///////////////////////////////////////////////////////////////////////////////
}
