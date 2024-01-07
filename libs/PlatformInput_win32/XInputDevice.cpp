#include "stdafx.h"
#include "XInputDevice.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"
#include "core_platform/inc/xinput_inc.h"
#pragma comment( lib, "Xinput.lib" )

#include "core/ptr/default_creator.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static rftl::string CreateDeviceName( XInputDevice::UserIndex userIndex )
{
	RF_ASSERT( userIndex <= XInputDevice::kMaxUserIndex );
	return rftl::string( "XInput" ) + math::integer_cast<char>( char( '0' ) + userIndex );
}



static win32::XINPUT_GAMEPAD CreateDisconnectedState()
{
	// Conveniently, zero is ideal for all inputs:
	//  * Zeroed buttons mean unpressed
	//  * Zeroed triggers mean unheld
	//  * Zeroed sticks mean centered
	win32::XINPUT_GAMEPAD retVal = {};
	return retVal;
}



DigitalPinState GetPhysicalState( win32::XINPUT_GAMEPAD const& state, PhysicalCode code )
{
	RF_ASSERT( code < math::enum_bitcast( XInputDigitalInputComponent::Pin::NumPins ) );
	win32::WORD const mask = math::integer_cast<win32::WORD>( 0b1 << code );
	bool const bit = ( state.wButtons & mask ) != 0;
	return bit ? DigitalPinState::Active : DigitalPinState::Inactive;
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
	mDigital.mDevice = this;
	mAnalog.mDevice = this;

	static_assert(
		XUSER_MAX_COUNT == kMaxUserIndex + 1,
		"XInput now supports more than 4 controllers?" );
}



void XInputDevice::CommonTick()
{
	static_assert( kStateBufferSize == sizeof( win32::XINPUT_GAMEPAD ), "Buffer size mismatch" );
	win32::XINPUT_GAMEPAD& previousState = *reinterpret_cast<win32::XINPUT_GAMEPAD*>( mPreviousStateBuffer.data() );
	win32::XINPUT_GAMEPAD& currentState = *reinterpret_cast<win32::XINPUT_GAMEPAD*>( mCurrentStateBuffer.data() );

	// Fetch state
	bool connected = false;
	win32::XINPUT_STATE newState = {};
	{
		win32::DWORD const stateResult = win32::XInputGetState(
			math::integer_cast<win32::DWORD>( mUserIndex ), &newState );
		if( stateResult == ERROR_SUCCESS )
		{
			// Success
			connected = true;
		}
		else if( stateResult == ERROR_DEVICE_NOT_CONNECTED )
		{
			// Not connected
			connected = false;
			newState.Gamepad = details::CreateDisconnectedState();
		}
		else
		{
			// Error
			RF_DBGFAIL_MSG( "Unknown error from XInputGetState(...)" );
			connected = false;
			newState.Gamepad = details::CreateDisconnectedState();
		}
	}

	// Increment state
	previousState = currentState;
	mPreviouslyConnected = mCurrentlyConnected;
	currentState = newState.Gamepad;
	mCurrentlyConnected = connected;

	// Handle delta logic
	if( previousState.wButtons == currentState.wButtons )
	{
		// No change, trivial ignore
	}
	else
	{
		win32::WORD const adds = math::integer_cast<win32::WORD>( ( ~previousState.wButtons ) & currentState.wButtons );
		win32::WORD const removes = math::integer_cast<win32::WORD>( previousState.wButtons & ( ~currentState.wButtons ) );
		RF_ASSERT( ( adds != 0 ) || ( removes != 0 ) );

		static constexpr size_t kNumPins = math::enum_bitcast( XInputDigitalInputComponent::Pin::NumPins );
		if( adds != 0 )
		{
			for( PhysicalCode pinAsCode = 0; pinAsCode < kNumPins; pinAsCode++ )
			{
				bool const changed = ( adds & ( 0b1 << pinAsCode ) ) != 0;
				if( changed )
				{
					mDigital.mPhysicalEventBuffer.emplace_back( pinAsCode, DigitalPinState::Active );
					mDigital.mLogicalEventBuffer.emplace_back( pinAsCode, DigitalPinState::Active );
				}
			}
		}
		if( removes != 0 )
		{
			for( PhysicalCode pinAsCode = 0; pinAsCode < kNumPins; pinAsCode++ )
			{
				bool const changed = ( removes & ( 0b1 << pinAsCode ) ) != 0;
				if( changed )
				{
					mDigital.mPhysicalEventBuffer.emplace_back( pinAsCode, DigitalPinState::Inactive );
					mDigital.mLogicalEventBuffer.emplace_back( pinAsCode, DigitalPinState::Inactive );
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

XInputDigitalInputComponent::XInputDigitalInputComponent() = default;



void XInputDigitalInputComponent::OnTick()
{
	// Updates are done in the common tick
}



PhysicalCode XInputDigitalInputComponent::GetMaxPhysicalCode() const
{
	return math::integer_cast<PhysicalCode>( math::enum_bitcast( Pin::NumPins ) - 1 );
}



LogicalCode XInputDigitalInputComponent::GetMaxLogicalCode() const
{
	return GetMaxPhysicalCode();
}



rftl::u16string XInputDigitalInputComponent::GetLogicalName( LogicalCode code ) const
{
	RF_ASSERT( code < math::enum_bitcast( XInputDigitalInputComponent::Pin::NumPins ) );
	switch( math::enum_bitcast<XInputDigitalInputComponent::Pin>( code ) )
	{
		case XInputDigitalInputComponent::Pin::DPadUp:
			return u"Up";
		case XInputDigitalInputComponent::Pin::DPadDown:
			return u"Down";
		case XInputDigitalInputComponent::Pin::DPadLeft:
			return u"Left";
		case XInputDigitalInputComponent::Pin::DPadRight:
			return u"Right";
		case XInputDigitalInputComponent::Pin::Start:
			return u"Start";
		case XInputDigitalInputComponent::Pin::Back:
			return u"Back";
		case XInputDigitalInputComponent::Pin::LeftThumb:
			return u"LS";
		case XInputDigitalInputComponent::Pin::RightThumb:
			return u"RS";
		case XInputDigitalInputComponent::Pin::LeftShoulder:
			return u"LB";
		case XInputDigitalInputComponent::Pin::RightShoulder:
			return u"RB";
		case XInputDigitalInputComponent::Pin::Reserved10:
			return u"RESERVED_10";
		case XInputDigitalInputComponent::Pin::Reserved11:
			return u"RESERVED_11";
		case XInputDigitalInputComponent::Pin::A:
			return u"A";
		case XInputDigitalInputComponent::Pin::B:
			return u"B";
		case XInputDigitalInputComponent::Pin::X:
			return u"X";
		case XInputDigitalInputComponent::Pin::Y:
			return u"Y";
		case XInputDigitalInputComponent::Pin::NumPins:
		default:
			RF_DBGFAIL();
			return u"INVALID";
	}
}



DigitalPinState XInputDigitalInputComponent::GetCurrentPhysicalState( PhysicalCode code ) const
{
	RF_ASSERT( mDevice != nullptr );
	win32::XINPUT_GAMEPAD const& currentState = *reinterpret_cast<win32::XINPUT_GAMEPAD const*>( mDevice->mCurrentStateBuffer.data() );
	return details::GetPhysicalState( currentState, code );
}



DigitalPinState XInputDigitalInputComponent::GetPreviousPhysicalState( PhysicalCode code ) const
{
	RF_ASSERT( mDevice != nullptr );
	win32::XINPUT_GAMEPAD const& previousState = *reinterpret_cast<win32::XINPUT_GAMEPAD const*>( mDevice->mPreviousStateBuffer.data() );
	return details::GetPhysicalState( previousState, code );
}



DigitalPinState XInputDigitalInputComponent::GetCurrentLogicalState( LogicalCode code ) const
{
	return GetCurrentPhysicalState( code );
}



DigitalPinState XInputDigitalInputComponent::GetPreviousLogicalState( LogicalCode code ) const
{
	return GetPreviousPhysicalState( code );
}



void XInputDigitalInputComponent::GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser, size_t maxEvents ) const
{
	size_t const numToRead = math::Min( mPhysicalEventBuffer.size(), maxEvents );
	for( size_t i = mPhysicalEventBuffer.size() - numToRead; i < mPhysicalEventBuffer.size(); i++ )
	{
		parser( mPhysicalEventBuffer[i] );
	}
}



void XInputDigitalInputComponent::GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser, size_t maxEvents ) const
{
	size_t const numToRead = math::Min( mLogicalEventBuffer.size(), maxEvents );
	for( size_t i = mLogicalEventBuffer.size() - numToRead; i < mLogicalEventBuffer.size(); i++ )
	{
		parser( mLogicalEventBuffer[i] );
	}
}



void XInputDigitalInputComponent::ClearPhysicalEventStream()
{
	mPhysicalEventBuffer.clear();
}



void XInputDigitalInputComponent::ClearLogicalEventStream()
{
	mLogicalEventBuffer.clear();
}

///////////////////////////////////////////////////////////////////////////////

XInputAnalogInputComponent::XInputAnalogInputComponent() = default;



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
