#pragma once
#include "core_input/DigitalInputComponent.h"
#include "core_input/AnalogInputComponent.h"
#include "core_input/TextInputComponent.h"

#include "core/ptr/unique_ptr.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

class InputDevice
{
	RF_NO_COPY( InputDevice );

public:
	void OnTick();
	virtual ~InputDevice() = default;

	UniquePtr<DigitalInputComponent> const mDigitalComponent;
	UniquePtr<AnalogInputComponent> const mAnalogComponent;
	UniquePtr<TextInputComponent> const mTextComponent;

protected:
	InputDevice() = delete;
	InputDevice(
		UniquePtr<DigitalInputComponent>&& digital,
		UniquePtr<AnalogInputComponent>&& analog,
		UniquePtr<TextInputComponent>&& text );
};

///////////////////////////////////////////////////////////////////////////////
}
