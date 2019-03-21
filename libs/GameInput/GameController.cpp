#include "stdafx.h"
#include "GameController.h"

#include "rftl/limits"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

void GameController::GetGameCommandStream( rftl::virtual_iterator<Command>& parser ) const
{
	return GetGameCommandStream( parser, rftl::numeric_limits<size_t>::max() );
}



void GameController::GetGameCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const
{
	//
}



void GameController::GetGameCommandStream( rftl::virtual_iterator<Command>& parser, time::FrameClock::time_point earliestTime ) const
{
	auto const onElement = [&parser, &earliestTime]( GameController::Command const& element ) -> void
	{
		if( element.mTime >= earliestTime )
		{
			parser( element );
		}
	};
	rftl::virtual_callable_iterator<GameController::Command, decltype( onElement )> filter( onElement );
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



void GameController::GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, GameSignalType type ) const
{
	return GetGameSignalStream( sampler, rftl::numeric_limits<size_t>::max(), type );
}



void GameController::GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, GameSignalType type ) const
{
	//
}



void GameController::GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, time::FrameClock::time_point earliestTime, GameSignalType type ) const
{
	auto const onElement = [&sampler, &earliestTime]( GameController::Signal const& element ) -> void
	{
		if( element.mTime >= earliestTime )
		{
			sampler( element );
		}
	};
	rftl::virtual_callable_iterator<GameController::Signal, decltype( onElement )> filter( onElement );
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
