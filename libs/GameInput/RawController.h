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

	void GetRawCommandStream( rftl::virtual_iterator<Command>& parser ) const;
	virtual void GetRawCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const = 0;
	void GetRawCommandStream( rftl::virtual_iterator<Command>& parser, time::FrameClock::time_point earliestTime ) const;
	void GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter ) const;
	virtual void GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter, size_t maxTypes ) const = 0;
	void GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, RawSignalType type ) const;
	virtual void GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, RawSignalType type ) const = 0;
	void GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, time::FrameClock::time_point earliestTime, RawSignalType type ) const;
	void GetTextStream( rftl::u16string& text ) const;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
