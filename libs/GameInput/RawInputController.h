#pragma once
#include "GameInput/RawController.h"

#include "core_input/InputFwd.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/unordered_map"
#include "rftl/deque"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class GAMEINPUT_API RawInputController final : public RawController
{
	RF_NO_COPY( RawInputController );


	//
	// Types
public:
	using PhysicalMapping = rftl::unordered_multimap<PhysicalCode, rftl::pair<DigitalPinState, RawCommandType>> ;
	using LogicalMapping = rftl::unordered_multimap<LogicalCode, rftl::pair<DigitalPinState, RawCommandType>>;
	using SignalMapping = rftl::unordered_map<AnalogSignalIndex, RawSignalType>;
private:
	using CommandBuffer = rftl::deque<Command>;
	using SignalBuffer = rftl::deque<Signal>;
	using SignalBufferMap = rftl::unordered_map<RawSignalType, SignalBuffer>;
	using TextBuffer = rftl::deque<char16_t>;


	//
	// Public methods
public:
	RawInputController() = default;
	virtual ~RawInputController() = default;

	void SetPhysicalMapping( PhysicalMapping const& mapping );
	void SetLogicalMapping( LogicalMapping const& mapping );
	void SetSignalMapping( SignalMapping const& mapping );

	void ConsumeInput( InputDevice& inputDevice );

	virtual void GetRawCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const override;
	virtual void GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter, size_t maxTypes ) const override;
	virtual void GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, RawSignalType type ) const override;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const override;
	virtual void ClearTextStream() override;


	//
	// Private data
private:
	PhysicalMapping mPhysicalMapping;
	LogicalMapping mLogicalMapping;
	SignalMapping mSignalMapping;

	CommandBuffer mCommandBuffer;
	SignalBufferMap mSignalBufferMap;
	TextBuffer mTextBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}}