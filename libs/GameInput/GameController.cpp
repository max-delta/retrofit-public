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
