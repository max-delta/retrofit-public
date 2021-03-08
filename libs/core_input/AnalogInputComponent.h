#pragma once
#include "core_input/InputComponent.h"

#include "rftl/string"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

class AnalogInputComponent : public InputComponent
{
	RF_NO_COPY( AnalogInputComponent );

public:
	AnalogInputComponent() = default;
	virtual void OnTick() override = 0;
	virtual InputComponentType GetType() override final
	{
		return InputComponentType::Analog;
	}

	virtual AnalogSignalIndex GetMaxSignalIndex() const = 0;

	virtual rftl::u16string GetSignalName( AnalogSignalIndex signalIndex ) const = 0;

	virtual AnalogSignalValue GetCurrentSignalValue( AnalogSignalIndex signalIndex ) const = 0;
	virtual AnalogSignalValue GetPreviousSignalValue( AnalogSignalIndex signalIndex ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
