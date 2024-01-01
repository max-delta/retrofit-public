#include "stdafx.h"
#include "GameController.h"

#include "rftl/limits"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

void GameController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser ) const
{
	return GetGameCommandStream( parser, rftl::numeric_limits<size_t>::max() );
}



void GameController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const
{
	//
}



void GameController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) const
{
	auto const onElement = [&parser, &earliestTime, &latestTime]( GameCommand const& element ) -> void {
		if( element.mTime >= earliestTime && element.mTime <= latestTime )
		{
			parser( element );
		}
	};
	rftl::virtual_callable_iterator<GameCommand, decltype( onElement )> filter( onElement );
	GetGameCommandStream( filter );
}



void GameController::GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter ) const
{
	return GetKnownSignals( iter, rftl::numeric_limits<size_t>::max() );
}



void GameController::GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter, size_t maxTypes ) const
{
	//
}



void GameController::GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, GameSignalType type ) const
{
	return GetGameSignalStream( sampler, rftl::numeric_limits<size_t>::max(), type );
}



void GameController::GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, size_t maxSamples, GameSignalType type ) const
{
	//
}



void GameController::GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime, GameSignalType type ) const
{
	auto const onElement = [&sampler, &earliestTime, &latestTime]( GameSignal const& element ) -> void {
		if( element.mTime >= earliestTime && element.mTime <= latestTime )
		{
			sampler( element );
		}
	};
	rftl::virtual_callable_iterator<GameSignal, decltype( onElement )> filter( onElement );
	GetGameSignalStream( filter, type );
}

///////////////////////////////////////////////////////////////////////////////

GameController::GameController( rftl::string&& identifier )
	: mIdentifier( identifier )
{
	RF_ASSERT( mIdentifier.empty() == false );
}

///////////////////////////////////////////////////////////////////////////////
}
