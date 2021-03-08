#pragma once
#include "GameInput/InputFwd.h"

#include "core_time/CommonClock.h"

#include "core/macros.h"

#include "rftl/extension/virtual_iterator.h"
#include "rftl/string"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

struct GAMEINPUT_API GameCommand
{
	GameCommandType mType;
	time::CommonClock::time_point mTime;
};

///////////////////////////////////////////////////////////////////////////////

struct GAMEINPUT_API GameSignal
{
	GameSignalValue mValue;
	time::CommonClock::time_point mTime;
};

///////////////////////////////////////////////////////////////////////////////

class GAMEINPUT_API GameController
{
	RF_NO_COPY( GameController );

	//
	// Public methods
public:
	GameController() = default;
	virtual ~GameController() = default;

	void GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser ) const;
	virtual void GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const;
	void GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) const;
	void GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter ) const;
	virtual void GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter, size_t maxTypes ) const;
	void GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, GameSignalType type ) const;
	virtual void GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, size_t maxSamples, GameSignalType type ) const;
	void GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime, GameSignalType type ) const;

	virtual void TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
