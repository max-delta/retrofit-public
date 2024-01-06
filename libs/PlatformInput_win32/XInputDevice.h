#pragma once
#include "project.h"

#include "core_input/InputDevice.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

class XInputDigitalInputComponent;
class XInputAnalogInputComponent;

// XBox 360 controller, and absolutely nothing else. Great for shitty PC
//  ports or generally looking like a budget indie studio...
class PLATFORMINPUT_API XInputDevice : public InputDevice
{
	RF_NO_COPY( XInputDevice );

public:
	using UserIndex = uint8_t;
	static constexpr UserIndex kMaxUserIndex = 3;

public:
	explicit XInputDevice( UserIndex userIndex );


public:
	UserIndex const mUserIndex;

	XInputDigitalInputComponent& mDigital;
	XInputAnalogInputComponent& mAnalog;
};

///////////////////////////////////////////////////////////////////////////////

class PLATFORMINPUT_API XInputDigitalInputComponent final : public DigitalInputComponent
{
	RF_NO_COPY( XInputDigitalInputComponent );

public:
	friend class XInputDevice;
public:
	XInputDigitalInputComponent() = default;
	virtual void OnTick() override;
	virtual PhysicalCode GetMaxPhysicalCode() const override;
	virtual LogicalCode GetMaxLogicalCode() const override;
	virtual rftl::u16string GetLogicalName( LogicalCode code ) const override;
	virtual DigitalPinState GetCurrentPhysicalState( PhysicalCode code ) const override;
	virtual DigitalPinState GetPreviousPhysicalState( PhysicalCode code ) const override;
	virtual DigitalPinState GetCurrentLogicalState( LogicalCode code ) const override;
	virtual DigitalPinState GetPreviousLogicalState( LogicalCode code ) const override;
	virtual void GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser, size_t maxEvents ) const override;
	virtual void GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser, size_t maxEvents ) const override;
	virtual void ClearPhysicalEventStream() override;
	virtual void ClearLogicalEventStream() override;
};

///////////////////////////////////////////////////////////////////////////////

class PLATFORMINPUT_API XInputAnalogInputComponent final : public AnalogInputComponent
{
	RF_NO_COPY( XInputAnalogInputComponent );

public:
	XInputAnalogInputComponent() = default;
	virtual void OnTick() override;
	virtual AnalogSignalIndex GetMaxSignalIndex() const override;
	virtual rftl::u16string GetSignalName( AnalogSignalIndex signalIndex ) const override;
	virtual AnalogSignalValue GetCurrentSignalValue( AnalogSignalIndex signalIndex ) const override;
	virtual AnalogSignalValue GetPreviousSignalValue( AnalogSignalIndex signalIndex ) const override;
};

///////////////////////////////////////////////////////////////////////////////
}
