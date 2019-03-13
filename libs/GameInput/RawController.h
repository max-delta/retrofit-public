#pragma once
#include "GameInput/InputFwd.h"

#include "Timing/clocks.h"

#include "core/macros.h"

#include "rftl/extension/virtual_iterator.h"
#include "rftl/string"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class GAMEINPUT_API RawController
{
	RF_NO_COPY( RawController );

	//
	// Structs
public:
	struct Command
	{
		RawCommandType mType;
		time::FrameClock::time_point mTime;
	};

	struct Signal
	{
		RawSignalValue mValue;
		time::FrameClock::time_point mTime;
	};


	//
	// Public methods
public:
	RawController() = default;
	virtual ~RawController() = default;
	virtual void GetRawCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const = 0;
	virtual void GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter, size_t maxTypes ) const = 0;
	virtual void GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, RawSignalType type ) const = 0;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const = 0;
	virtual void ClearTextStream() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
