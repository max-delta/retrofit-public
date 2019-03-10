#pragma once
#include "GameInput/InputFwd.h"

#include "Timing/clocks.h"

#include "rftl/extension/virtual_iterator.h"
#include "rftl/string"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class GAMEINPUT_API GameController
{
	//
	// Structs
public:
	struct Command
	{
		GameCommandType mType;
		time::FrameClock::time_point mTime;
	};

	struct Signal
	{
		GameSignalValue mValue;
		time::FrameClock::time_point mTime;
	};


	//
	// Public methods
public:
	virtual ~GameController() = default;
	virtual void GetGameCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const = 0;
	virtual void GetGameSignalStream( rftl::virtual_iterator<Signal>& parser, size_t maxSamples, GameSignalType type ) const = 0;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const = 0;
	virtual void ClearTextStream() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
