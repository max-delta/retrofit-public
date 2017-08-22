#pragma once
#include "core/ptr/unique_ptr.h"

#include <stdint.h>
#include <iterator>
#include <vector>


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

template<typename EventType>
struct EventParser
{
	virtual void OnEvent( EventType const& event ) const = 0;
};

template<typename EventType, typename Container>
struct BufferCopyEventParser : public EventParser
{
	BufferCopyEventParser( Container& containerRef )
		: m_Inserter( containerRef )
	{
		//
	}

	virtual void OnEvent( EventType const& event ) const
	{
		m_Inserter( event );
	}

	std::back_insert_iterator<Container> const m_Inserter;
};



class InputComponent
{
public:
	enum ComponentType : uint8_t
	{
		Invalid = 0,
		Digital,
		Analog,
		Text
	};

	virtual void OnTick() = 0;
	virtual ComponentType GetType() = 0;
};



class DigitalInputComponent : public InputComponent
{
public:
	typedef uint8_t PhysicalCode;
	typedef uint8_t LogicalCode;

	enum PinState : bool
	{
		Active = true,
		Inactive = false
	};
	virtual void OnTick() override = 0;
	virtual ComponentType GetType() override final
	{
		return ComponentType::Digital;
	}

	virtual PhysicalCode GetMaxPhysicalCode() const = 0;
	virtual LogicalCode GetMaxLogicalCode() const = 0;

	virtual std::u16string GetLogicalName( LogicalCode code ) const = 0;

	virtual PinState GetCurrentPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetPreviousPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetCurrentLogicalState( LogicalCode code ) const = 0;
	virtual PinState GetPreviousLogicalState( LogicalCode code ) const = 0;

	struct PhysicalEvent
	{
		PhysicalCode m_Code;
		PinState m_NewState;
	};
	struct LogicalEvent
	{
		LogicalCode m_Code;
		PinState m_NewState;
	};

	virtual void GetPhysicalEventStream( EventParser<PhysicalEvent>& parser, size_t maxEvents ) const = 0;
	virtual void GetLogicalEventStream( EventParser<LogicalEvent>& parser, size_t maxEvents ) const = 0;

	virtual void ClearPhysicalEventStream() = 0;
	virtual void ClearLogicalEventStream() = 0;
};



class TextInputComponent : public InputComponent
{
public:
	virtual void OnTick() override = 0;
	virtual ComponentType GetType() override final
	{
		return ComponentType::Text;
	}

	virtual void GetTextStream( std::u16string& text, size_t maxLen ) const = 0;
	virtual void ClearTextStream() = 0;
};



class AnalogInputComponent : public InputComponent
{
public:
	typedef uint8_t SignalIndex;
	typedef float SignalValue;

	static constexpr SignalValue k_MaxSignalValue = 1.0f;
	static constexpr SignalValue k_MinSignalValue = 0.0f;

	virtual void OnTick() override = 0;
	virtual ComponentType GetType() override final
	{
		return ComponentType::Analog;
	}

	virtual SignalIndex GetMaxSignalIndex() const = 0;

	virtual std::u16string GetSignalName( SignalIndex signalIndex ) const = 0;

	virtual SignalValue GetCurrentSignalValue( SignalIndex signalIndex ) const = 0;
	virtual SignalValue GetPreviousSignalValue( SignalIndex signalIndex ) const = 0;
};



class InputDevice
{
public:
	void OnTick();
	virtual ~InputDevice() = default;

	UniquePtr<DigitalInputComponent> const m_DigitalComponent;
	UniquePtr<AnalogInputComponent> const m_AnalogComponent;
	UniquePtr<TextInputComponent> const m_TextComponent;

protected:
	InputDevice() = delete;
	InputDevice(
		UniquePtr<DigitalInputComponent> && digital,
		UniquePtr<AnalogInputComponent> && analog,
		UniquePtr<TextInputComponent> && text );
};



class NullInputDevice : public InputDevice
{
public:
	NullInputDevice();
};
class WndProcInputDevice : public InputDevice
{
public:
	WndProcInputDevice();
	// Easiest, best behavior. If you don't use for text entry and cursor
	//  control, it's probably because you're an idiot...
	// NOTE: Raw input for mouse movement will be flipped on and processed as
	//  seperate relative analog input in here. Responsible developers will
	//  avoid using this unless they've got a reasonable case where they want
	//  to break support for most user input devices (for example, if you are
	//  an FPS that really only works with relative positioning devices, and
	//  also have a community that demands sub-pixel DPI motion)
};
class DirectInputDevice : public InputDevice
{
public:
	DirectInputDevice();
	// Most compatible behavior, but basically mandates an input remapping gui,
	//  so your users can map whatever the fuck the '_Rxz2' axis is supposed to
	//  mean on their uber-joystick from the 90s
};
class XInputDevice : public InputDevice
{
public:
	XInputDevice();
	// XBox 360 controller, and absolutely nothing else. Great for shitty PC
	//  ports or generally looking like a budget indie studio...
};

#error WIP, still just sketching out ideas

///////////////////////////////////////////////////////////////////////////////
}}
