#pragma once
#include "core_input/InputComponent.h"

#include "rftl/string"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class AnalogInputComponent : public InputComponent
{
	RF_NO_COPY( AnalogInputComponent );

public:
	typedef uint8_t SignalIndex;
	typedef float SignalValue;

	static constexpr SignalValue kMaxSignalValue = 1.0f;
	static constexpr SignalValue kMinSignalValue = 0.0f;

	AnalogInputComponent() = default;
	virtual void OnTick() override = 0;
	virtual ComponentType GetType() override final
	{
		return ComponentType::Analog;
	}

	virtual SignalIndex GetMaxSignalIndex() const = 0;

	virtual rftl::u16string GetSignalName( SignalIndex signalIndex ) const = 0;

	virtual SignalValue GetCurrentSignalValue( SignalIndex signalIndex ) const = 0;
	virtual SignalValue GetPreviousSignalValue( SignalIndex signalIndex ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
