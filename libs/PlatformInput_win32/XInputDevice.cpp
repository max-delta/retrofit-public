#include "stdafx.h"
#include "XInputDevice.h"

#include "core/ptr/default_creator.h"
#include "core_math/math_casts.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static rftl::string CreateDeviceName( XInputDevice::UserIndex userIndex )
{
	RF_ASSERT( userIndex <= XInputDevice::kMaxUserIndex );
	return rftl::string( "XInput" ) + math::integer_cast<char>( char( '0' ) + userIndex );
}

}
///////////////////////////////////////////////////////////////////////////////

XInputDevice::XInputDevice( UserIndex userIndex )
	: //
	InputDevice(
		details::CreateDeviceName( userIndex ),
		DefaultCreator<XInputDigitalInputComponent>::Create(),
		DefaultCreator<XInputAnalogInputComponent>::Create(),
		nullptr )
	, mUserIndex( userIndex )
	, mDigital( *reinterpret_cast<XInputDigitalInputComponent*>( mDigitalComponent.Get() ) )
	, mAnalog( *reinterpret_cast<XInputAnalogInputComponent*>( mAnalogComponent.Get() ) )
{
	RF_ASSERT( mUserIndex <= XInputDevice::kMaxUserIndex );
}

///////////////////////////////////////////////////////////////////////////////

void XInputDigitalInputComponent::OnTick()
{
	RF_TODO_ANNOTATION( "Implement" );
}



PhysicalCode XInputDigitalInputComponent::GetMaxPhysicalCode() const
{
	RF_TODO_ANNOTATION( "Implement" );
	return 0;
}



LogicalCode XInputDigitalInputComponent::GetMaxLogicalCode() const
{
	RF_TODO_ANNOTATION( "Implement" );
	return 0;
}



rftl::u16string XInputDigitalInputComponent::GetLogicalName( LogicalCode code ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return u"INVALID";
}



DigitalPinState XInputDigitalInputComponent::GetCurrentPhysicalState( PhysicalCode code ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return DigitalPinState::Inactive;
}



DigitalPinState XInputDigitalInputComponent::GetPreviousPhysicalState( PhysicalCode code ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return DigitalPinState::Inactive;
}



DigitalPinState XInputDigitalInputComponent::GetCurrentLogicalState( LogicalCode code ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return DigitalPinState::Inactive;
}



DigitalPinState XInputDigitalInputComponent::GetPreviousLogicalState( LogicalCode code ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return DigitalPinState::Inactive;
}



void XInputDigitalInputComponent::GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser, size_t maxEvents ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return;
}



void XInputDigitalInputComponent::GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser, size_t maxEvents ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return;
}



void XInputDigitalInputComponent::ClearPhysicalEventStream()
{
	RF_TODO_ANNOTATION( "Implement" );
}



void XInputDigitalInputComponent::ClearLogicalEventStream()
{
	RF_TODO_ANNOTATION( "Implement" );
}

///////////////////////////////////////////////////////////////////////////////

void XInputAnalogInputComponent::OnTick()
{
	RF_TODO_ANNOTATION( "Implement" );
}



AnalogSignalIndex XInputAnalogInputComponent::GetMaxSignalIndex() const
{
	RF_TODO_ANNOTATION( "Implement" );
	return 0;
}



rftl::u16string XInputAnalogInputComponent::GetSignalName( AnalogSignalIndex signalIndex ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return u"INVALID";
}



AnalogSignalValue XInputAnalogInputComponent::GetCurrentSignalValue( AnalogSignalIndex signalIndex ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return 0;
}



AnalogSignalValue XInputAnalogInputComponent::GetPreviousSignalValue( AnalogSignalIndex signalIndex ) const
{
	RF_TODO_ANNOTATION( "Implement" );
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
}
