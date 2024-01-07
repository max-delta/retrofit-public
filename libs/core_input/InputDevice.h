#pragma once
#include "core_input/DigitalInputComponent.h"
#include "core_input/AnalogInputComponent.h"
#include "core_input/TextInputComponent.h"

#include "core/ptr/unique_ptr.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

// Represents a device that has one or more types of input it can produce
class InputDevice
{
	RF_NO_COPY( InputDevice );

public:
	virtual ~InputDevice() = default;

	void OnTick();

	rftl::string const mIdentifier;
	UniquePtr<DigitalInputComponent> const mDigitalComponent;
	UniquePtr<AnalogInputComponent> const mAnalogComponent;
	UniquePtr<TextInputComponent> const mTextComponent;

protected:
	InputDevice() = delete;
	// NOTE: Identifier does not need to be unique (it is mostly for debug)
	InputDevice(
		rftl::string&& identifier,
		UniquePtr<DigitalInputComponent>&& digital,
		UniquePtr<AnalogInputComponent>&& analog,
		UniquePtr<TextInputComponent>&& text );

	virtual void CommonTick() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
