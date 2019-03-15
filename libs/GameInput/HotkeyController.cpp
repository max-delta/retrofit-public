#include "stdafx.h"
#include "HotkeyController.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

WeakPtr<RawController> HotkeyController::GetSource() const
{
	return mSource;
}



void HotkeyController::SetSource( WeakPtr<RawController> const& source )
{
	mSource = source;
}



void HotkeyController::GetGameCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const
{
	RF_TODO_BREAK();
}



void HotkeyController::GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter, size_t maxTypes ) const
{
	RF_TODO_BREAK();
}



void HotkeyController::GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, GameSignalType type ) const
{
	RF_TODO_BREAK();
}



void HotkeyController::GetTextStream( rftl::u16string& text, size_t maxLen ) const
{
	RF_TODO_BREAK();
}



void HotkeyController::ClearTextStream()
{
	RF_TODO_BREAK();
}

///////////////////////////////////////////////////////////////////////////////
}}
