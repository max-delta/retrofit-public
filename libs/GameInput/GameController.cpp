#include "stdafx.h"
#include "GameController.h"

#include "rftl/limits"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

void GameController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser ) const
{
	return GetGameCommandStream( parser, rftl::numeric_limits<size_t>::max() );
}



void GameController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const
{
	//
}



void GameController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, time::FrameClock::time_point earliestTime ) const
{
	auto const onElement = [&parser, &earliestTime]( GameCommand const& element ) -> void
	{
		if( element.mTime >= earliestTime )
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



void GameController::GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, time::FrameClock::time_point earliestTime, GameSignalType type ) const
{
	auto const onElement = [&sampler, &earliestTime]( GameSignal const& element ) -> void
	{
		if( element.mTime >= earliestTime )
		{
			sampler( element );
		}
	};
	rftl::virtual_callable_iterator<GameSignal, decltype( onElement )> filter( onElement );
	GetGameSignalStream( filter, type );
}



void GameController::GetTextStream( rftl::u16string& text ) const
{
	return GetTextStream( text, rftl::numeric_limits<size_t>::max() );
}



void GameController::GetTextStream( rftl::u16string& text, size_t maxLen ) const
{
	text.clear();
}

///////////////////////////////////////////////////////////////////////////////
}}
