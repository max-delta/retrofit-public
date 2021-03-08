#pragma once
#include "rftl/cstdint"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

enum InputComponentType : uint8_t
{
	Invalid = 0,
	Digital,
	Analog,
	Text
};

enum DigitalPinState : bool
{
	Active = true,
	Inactive = false
};

using PhysicalCode = uint8_t;
using LogicalCode = uint8_t;

using AnalogSignalIndex = uint8_t;
using AnalogSignalValue = float;
static constexpr AnalogSignalValue kMaxAnalogSignalValue = 1.0f;
static constexpr AnalogSignalValue kMinAnalogSignalValue = 0.0f;

class InputDevice;
class InputComponent;
class DigitalInputComponent;
class AnalogInputComponent;
class TextInputComponent;

///////////////////////////////////////////////////////////////////////////////
}
