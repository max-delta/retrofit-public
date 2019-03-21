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
	void GetGameCommandStream( rftl::virtual_iterator<Command>& parser ) const;
	virtual void GetGameCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const;
	void GetGameCommandStream( rftl::virtual_iterator<Command>& parser, time::FrameClock::time_point earliestTime ) const;
	void GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter ) const;
	virtual void GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter, size_t maxTypes ) const;
	void GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, GameSignalType type ) const;
	virtual void GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, GameSignalType type ) const;
	void GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, time::FrameClock::time_point earliestTime, GameSignalType type ) const;
	void GetTextStream( rftl::u16string& text ) const;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const;
};

///////////////////////////////////////////////////////////////////////////////
}}
