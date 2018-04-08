#include "stdafx.h"
#include "InputDevice.h"

namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

void InputDevice::OnTick()
{
	DigitalInputComponent* const digComp = m_DigitalComponent;
	AnalogInputComponent* const anaComp = m_AnalogComponent;
	TextInputComponent* const texComp = m_TextComponent;
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
	: m_DigitalComponent( rftl::move( digital ) )
	, m_AnalogComponent( rftl::move( analog ) )
	, m_TextComponent( rftl::move( text ) )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}}
