#pragma once
#include "core_input/DigitalInputComponent.h"
#include "core_input/AnalogInputComponent.h"
#include "core_input/TextInputComponent.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class InputDevice
{
	RF_NO_COPY( InputDevice );

public:
	void OnTick();
	virtual ~InputDevice() = default;

	UniquePtr<DigitalInputComponent> const m_DigitalComponent;
	UniquePtr<AnalogInputComponent> const m_AnalogComponent;
	UniquePtr<TextInputComponent> const m_TextComponent;

protected:
	InputDevice() = delete;
	InputDevice(
		UniquePtr<DigitalInputComponent>&& digital,
		UniquePtr<AnalogInputComponent>&& analog,
		UniquePtr<TextInputComponent>&& text );
};

///////////////////////////////////////////////////////////////////////////////
}}
