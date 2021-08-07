#include "stdafx.h"
#include "InputDevice.h"

namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

void InputDevice::OnTick()
{
	DigitalInputComponent* const digComp = mDigitalComponent;
	AnalogInputComponent* const anaComp = mAnalogComponent;
	TextInputComponent* const texComp = mTextComponent;
	if( digComp != nullptr )
	{
		digComp->OnTick();
	}
	if( anaComp != nullptr )
	{
		anaComp->OnTick();
	}
	if( texComp != nullptr )
	{
		texComp->OnTick();
	}
}



InputDevice::InputDevice(
	UniquePtr<DigitalInputComponent>&& digital,
	UniquePtr<AnalogInputComponent>&& analog,
	UniquePtr<TextInputComponent>&& text )
	: mDigitalComponent( rftl::move( digital ) )
	, mAnalogComponent( rftl::move( analog ) )
	, mTextComponent( rftl::move( text ) )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}
