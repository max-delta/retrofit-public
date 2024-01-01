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
};

///////////////////////////////////////////////////////////////////////////////
}
