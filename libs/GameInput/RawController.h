#pragma once
#include "GameInput/InputFwd.h"

#include "core_time/CommonClock.h"

#include "core/macros.h"

#include "rftl/extension/virtual_iterator.h"
#include "rftl/string"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

struct GAMEINPUT_API RawCommand
{
	RawCommandType mType;
	time::CommonClock::time_point mTime;
};

///////////////////////////////////////////////////////////////////////////////

struct GAMEINPUT_API RawSignal
{
	RawSignalValue mValue;
	time::CommonClock::time_point mTime;
};

///////////////////////////////////////////////////////////////////////////////

// Interface for low-level controllers to produce generic input for later
//  post-processing to be done by game controllers
// EXAMPLE: A raw controller might emit "Home" and "End", and a game controller
//  might post-process that into "Navigate to first" and "Navigate to last"
// EXAMPLE: A raw controlelr might emit "+Down">"+Right">"-Down">"-Right", and
//  a game controller might post-process that into "Quarter-circle right"
class GAMEINPUT_API RawController
{
	RF_NO_COPY( RawController );

	//
	// Public methods
public:
	RawController() = default;
	virtual ~RawController() = default;

	void GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser ) const;
	virtual void GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser, size_t maxCommands ) const = 0;
	void GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) const;

	void GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter ) const;
	virtual void GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter, size_t maxTypes ) const = 0;
	void GetRawSignalStream( rftl::virtual_iterator<RawSignal>& sampler, RawSignalType type ) const;
	virtual void GetRawSignalStream( rftl::virtual_iterator<RawSignal>& sampler, size_t maxSamples, RawSignalType type ) const = 0;
	void GetRawSignalStream( rftl::virtual_iterator<RawSignal>& sampler, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime, RawSignalType type ) const;

	void GetTextStream( rftl::u16string& text ) const;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const = 0;
	virtual void ClearTextStream() = 0;

	virtual void TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
