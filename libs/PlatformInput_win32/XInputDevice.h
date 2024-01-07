#pragma once
#include "project.h"

#include "core_input/InputDevice.h"

#include "rftl/array"
#include "rftl/deque"


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
private:
	static constexpr size_t kStateBufferSize = 12;
	using StateBuffer = rftl::array<uint8_t, kStateBufferSize>;


public:
	explicit XInputDevice( UserIndex userIndex );


protected:
	virtual void CommonTick() override;


public:
	UserIndex const mUserIndex;

	StateBuffer mPreviousStateBuffer = {};
	StateBuffer mCurrentStateBuffer = {};
	bool mPreviouslyConnected = false;
	bool mCurrentlyConnected = false;

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
	// NOTE: These are bitshift-left distances into an underlying 16-bit field
	enum class Pin : uint8_t
	{
		DPadUp = 0,
		DPadDown = 1,
		DPadLeft = 2,
		DPadRight = 3,

		Start = 4,
		Back = 5,

		LeftThumb = 6,
		RightThumb = 7,

		LeftShoulder = 8,
		RightShoulder = 9,

		Reserved10 = 10,
		Reserved11 = 11,

		A = 12,
		B = 13,
		X = 14,
		Y = 15,

		NumPins,
	};
private:
	using LogicalEventBuffer = rftl::deque<LogicalEvent>;
	using PhysicalEventBuffer = rftl::deque<PhysicalEvent>;
public:
	XInputDigitalInputComponent();
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
private:
	// Would prefer to be reference, but initialization timing raises warnings
	XInputDevice const* mDevice = nullptr;
	LogicalEventBuffer mLogicalEventBuffer;
	PhysicalEventBuffer mPhysicalEventBuffer;
};

///////////////////////////////////////////////////////////////////////////////

class PLATFORMINPUT_API XInputAnalogInputComponent final : public AnalogInputComponent
{
	RF_NO_COPY( XInputAnalogInputComponent );

public:
	friend class XInputDevice;
public:
	enum class Signal : uint8_t
	{
		LT = 0,
		RT,

		LX,
		LY,

		RX,
		RY,

		NumSignals,
	};
public:
	XInputAnalogInputComponent();
	virtual void OnTick() override;
	virtual AnalogSignalIndex GetMaxSignalIndex() const override;
	virtual rftl::u16string GetSignalName( AnalogSignalIndex signalIndex ) const override;
	virtual AnalogSignalValue GetCurrentSignalValue( AnalogSignalIndex signalIndex ) const override;
	virtual AnalogSignalValue GetPreviousSignalValue( AnalogSignalIndex signalIndex ) const override;
private:
	// Would prefer to be reference, but initialization timing raises warnings
	XInputDevice const* mDevice = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
