#include "stdafx.h"
#include "WndProcInputDevice.h"

#include "core_platform/windows_inc.h"
#include "core/ptr/default_creator.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

WndProcInputDevice::WndProcInputDevice()
	: InputDevice(
		DefaultCreator<WndProcDigitalInputComponent>::Create(),
		DefaultCreator<WndProcAnalogInputComponent>::Create(),
		DefaultCreator<WndProcTextInputComponent>::Create() )
	, mDigital( *reinterpret_cast<WndProcDigitalInputComponent*>( mDigitalComponent.Get() ) )
	, mAnalog( *reinterpret_cast<WndProcAnalogInputComponent*>( mAnalogComponent.Get() ) )
	, mText( *reinterpret_cast<WndProcTextInputComponent*>( mTextComponent.Get() ) )
{
	//
}



shim::LRESULT WndProcInputDevice::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted )
{
	shim::LRESULT retVal = {};
	shim::LRESULT componentResult;
	bool componentIntercepted = false;

	componentResult = mDigital.ExamineTranslatedMessage( hWnd, message, wParam, lParam, componentIntercepted );
	if( componentIntercepted )
	{
		retVal = componentResult;
		intercepted = true;
	}

	componentResult = mAnalog.ExamineTranslatedMessage( hWnd, message, wParam, lParam, componentIntercepted );
	if( componentIntercepted )
	{
		retVal = componentResult;
		intercepted = true;
	}

	componentResult = mText.ExamineTranslatedMessage( hWnd, message, wParam, lParam, componentIntercepted );
	if( componentIntercepted )
	{
		retVal = componentResult;
		intercepted = true;
	}

	if( intercepted )
	{
		return retVal;
	}

	intercepted = false;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

void WndProcDigitalInputComponent::OnTick()
{
	mPreviousLogicalState = mCurrentLogicalState;
	mPreviousPhysicalState = mCurrentPhysicalState;
}



PhysicalCode WndProcDigitalInputComponent::GetMaxPhysicalCode() const
{
	RF_DBGFAIL_MSG( "TODO" );
	return PhysicalCode();
}



LogicalCode WndProcDigitalInputComponent::GetMaxLogicalCode() const
{
	RF_DBGFAIL_MSG( "TODO" );
	return LogicalCode();
}



rftl::u16string WndProcDigitalInputComponent::GetLogicalName( LogicalCode code ) const
{
	RF_DBGFAIL_MSG( "TODO" );
	return rftl::u16string();
}



DigitalPinState WndProcDigitalInputComponent::GetCurrentPhysicalState( PhysicalCode code ) const
{
	return mCurrentPhysicalState[code] ? DigitalPinState::Active : DigitalPinState::Inactive;
}



DigitalPinState WndProcDigitalInputComponent::GetPreviousPhysicalState( PhysicalCode code ) const
{
	return mPreviousPhysicalState[code] ? DigitalPinState::Active : DigitalPinState::Inactive;
}



DigitalPinState WndProcDigitalInputComponent::GetCurrentLogicalState( LogicalCode code ) const
{
	return mCurrentLogicalState[code] ? DigitalPinState::Active : DigitalPinState::Inactive;
}



DigitalPinState WndProcDigitalInputComponent::GetPreviousLogicalState( LogicalCode code ) const
{
	return mPreviousLogicalState[code] ? DigitalPinState::Active : DigitalPinState::Inactive;
}



void WndProcDigitalInputComponent::GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser, size_t maxEvents ) const
{
	size_t const numToRead = math::Min( mPhysicalEventBuffer.size(), maxEvents );
	for( size_t i = mPhysicalEventBuffer.size() - numToRead; i < mPhysicalEventBuffer.size(); i++ )
	{
		parser( mPhysicalEventBuffer[i] );
	}
}



void WndProcDigitalInputComponent::GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser, size_t maxEvents ) const
{
	size_t const numToRead = math::Min( mLogicalEventBuffer.size(), maxEvents );
	for( size_t i = mLogicalEventBuffer.size() - numToRead; i < mLogicalEventBuffer.size(); i++ )
	{
		parser( mLogicalEventBuffer[i] );
	}
}



void WndProcDigitalInputComponent::ClearPhysicalEventStream()
{
	RF_DBGFAIL_MSG( "TODO" );
}



void WndProcDigitalInputComponent::ClearLogicalEventStream()
{
	RF_DBGFAIL_MSG( "TODO" );
}

///////////////////////////////////////////////////////////////////////////////

shim::LRESULT WndProcDigitalInputComponent::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted )
{
	// TODO: Alt keys

	union KeyUpDown
	{
		KeyUpDown( shim::LPARAM value )
			: m_Value( value )
		{
		}
		shim::LPARAM m_Value;
		struct Fields
		{
			shim::LPARAM m_RepeatCount : 16;
			shim::LPARAM m_ScanCode : 8;
			shim::LPARAM m_Extended : 1;
			shim::LPARAM m_Reserved : 4;
			shim::LPARAM m_Context : 1;
			shim::LPARAM m_Previous : 1;
			shim::LPARAM m_Transition : 1;
			//shim::LPARAM Padding : 32; // Only on 64-bit
		} m_Fields;
	};
	static_assert( sizeof( KeyUpDown::Fields ) == sizeof( shim::LPARAM ), "" );
	static_assert( sizeof( KeyUpDown ) == sizeof( shim::LPARAM ), "" );

	switch( message )
	{
		// wParam: Virtual Key Code
		// lParam [bit 0-15] : Repeat count
		// lParam [bit 16-23] : Scan code
		// lParam [bit 24] : Extended key bit
		// lParam [bit 25-28] : Reserved
		// lParam [bit 29] : Context code
		// lParam [bit 30] : Previous state
		// lParam [bit 31] : Transition state
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint8_t const virtualKey = math::integer_cast<uint8_t>( wParam );
			uint8_t const scanCode = math::integer_cast<uint8_t>( KeyUpDown( lParam ).m_Fields.m_ScanCode );
			DigitalPinState state;
			switch( message )
			{
				case WM_KEYDOWN:
					state = DigitalPinState::Active;
					break;
				case WM_KEYUP:
					state = DigitalPinState::Inactive;
					break;
				default:
					RF_DBGFAIL();
					state = DigitalPinState::Inactive;
					break;
			}
			RecordLogical( virtualKey, state );
			RecordPhysical( scanCode, state );
			intercepted = true;
			return 0;
		}


		// wParam: Modifiers
		// lParam [bit 0-15] : Absolute x-coord in client area
		// lParam [bit 16-31] : Absolute y-coord in client area
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		{
			DigitalPinState state;
			switch( message )
			{
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_XBUTTONDOWN:
					state = DigitalPinState::Active;
					break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
				case WM_MBUTTONUP:
				case WM_XBUTTONUP:
					// NOTE: User can drag-escape window to hide this event
					state = DigitalPinState::Inactive;
					break;
				default:
					RF_DBGFAIL();
					state = DigitalPinState::Inactive;
					break;
			}

			uint8_t virtualKey;
			switch( message )
			{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
					virtualKey = VK_LBUTTON;
					break;
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
					virtualKey = VK_RBUTTON;
					break;
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
					virtualKey = VK_MBUTTON;
					break;
				case WM_XBUTTONDOWN:
				case WM_XBUTTONUP:
					if( ( wParam & MK_XBUTTON1 ) != 0 )
					{
						virtualKey = VK_XBUTTON1;
					}
					else if( ( wParam & MK_XBUTTON2 ) != 0 )
					{
						virtualKey = VK_XBUTTON2;
					}
					else
					{
						RF_DBGFAIL();
						virtualKey = 0;
					}
					break;
				default:
					RF_DBGFAIL();
					virtualKey = 0;
					break;
			}

			RecordLogical( virtualKey, state );
			intercepted = true;
			return 0;
		}


		// wParam [bit 0-15] : WHEEL_DELTA * num clicks (negative is down)
		// wParam [bit 16-31] : Modifiers
		// lParam [bit 0-15] : Absolute x-coord in client area
		// lParam [bit 16-31] : Absolute y-coord in client area
		case WM_MOUSEHWHEEL:
		{
			// TODO: This doesn't seem to actually work, or ever even get
			//  fired, which seems like perhaps the window has to steal input
			//  focus in order to receive the message

			int16_t const distance = static_cast<int32_t>( wParam ) >> 16;
			if( distance == 0 )
			{
				// Uh... Why did we even get a message?
				intercepted = false;
				return 0;
			}

			bool const positive = distance > 0;

			int16_t numClicks = distance / WHEEL_DELTA;
			if( numClicks < 0 )
			{
				// Num clicks will always be used as positive
				numClicks = -numClicks;
			}
			if( numClicks == 0 )
			{
				// API failure, try to correct
				numClicks = 1;
			}

			static constexpr int16_t kMaxReasonableClicks = 5;
			if( numClicks > kMaxReasonableClicks )
			{
				// Something seems to have gone wrong, try and clamp it down
				numClicks = kMaxReasonableClicks;
			}

			uint8_t const virtualKey = positive ? kVkScrollWheelUp : kVkScrollWheelDown;
			for( int16_t i = 0; i < numClicks; i++ )
			{
				// Pulse the key for each click
				RecordLogical( virtualKey, DigitalPinState::Active );
				RecordLogical( virtualKey, DigitalPinState::Inactive );
			}
			intercepted = true;
			return 0;
		}


		case WM_KILLFOCUS:
		{
			if( kKillKeysOnFocusLost == false )
			{
				intercepted = false;
				return 0;
			}

			size_t const numLogical = mCurrentLogicalState.size();
			for( size_t i = 0; i < numLogical; i++ )
			{
				bool const active = mCurrentLogicalState[i];
				if( active )
				{
					uint8_t const code = math::integer_cast<uint8_t>( i );
					RecordLogical( code, DigitalPinState::Inactive );
				}
			}

			size_t const numPhysical = mCurrentPhysicalState.size();
			for( size_t i = 0; i < numPhysical; i++ )
			{
				bool const active = mCurrentPhysicalState[i];
				if( active )
				{
					uint8_t const code = math::integer_cast<uint8_t>( i );
					RecordPhysical( code, DigitalPinState::Inactive );
				}
			}

			intercepted = true;
			return 0;
		}


		default:
		{
			intercepted = false;
			return 0;
		}
	}
}



void WndProcDigitalInputComponent::RecordLogical( uint8_t code, DigitalPinState state )
{
	if( mCurrentLogicalState[code] == ( state == DigitalPinState::Active ) )
	{
		// Repeat, ignore
		return;
	}

	mCurrentLogicalState[code] = state == DigitalPinState::Active;
	mLogicalEventBuffer.emplace_back( code, state );
	while( mLogicalEventBuffer.size() > kMaxEventStorage )
	{
		mLogicalEventBuffer.pop_front();
	}
}



void WndProcDigitalInputComponent::RecordPhysical( uint8_t code, DigitalPinState state )
{
	if( mCurrentPhysicalState[code] == ( state == DigitalPinState::Active ) )
	{
		// Repeat, ignore
		return;
	}

	mCurrentPhysicalState[code] = state == DigitalPinState::Active;
	mPhysicalEventBuffer.emplace_back( code, state );
	while( mPhysicalEventBuffer.size() > kMaxEventStorage )
	{
		mPhysicalEventBuffer.pop_front();
	}
}

///////////////////////////////////////////////////////////////////////////////

void WndProcAnalogInputComponent::OnTick()
{
	static_assert( rftl::is_array<SignalValues>::value, "Type changed, re-evalute memcpy safety" );
	memcpy( mPreviousSignalValues, mCurrentSignalValues, sizeof( SignalValues ) );
}



AnalogSignalIndex WndProcAnalogInputComponent::GetMaxSignalIndex() const
{
	return k_NumSignals;
}



rftl::u16string WndProcAnalogInputComponent::GetSignalName( AnalogSignalIndex signalIndex ) const
{
	RF_DBGFAIL_MSG( "TODO" );
	return rftl::u16string();
}



AnalogSignalValue WndProcAnalogInputComponent::GetCurrentSignalValue( AnalogSignalIndex signalIndex ) const
{
	RF_ASSERT_MSG( signalIndex < GetMaxSignalIndex(), "Invalid parameter" );
	return mCurrentSignalValues[signalIndex];
}



AnalogSignalValue WndProcAnalogInputComponent::GetPreviousSignalValue( AnalogSignalIndex signalIndex ) const
{
	RF_ASSERT_MSG( signalIndex < GetMaxSignalIndex(), "Invalid parameter" );
	return mPreviousSignalValues[signalIndex];
}

///////////////////////////////////////////////////////////////////////////////

shim::LRESULT WndProcAnalogInputComponent::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted )
{
	// TODO: Relative velocity from wheel
	// TODO: Relative velocity from raw mouse

	union MouseCoord
	{
		MouseCoord( shim::LPARAM value )
			: m_Value( value )
		{
		}
		shim::LPARAM m_Value;
		struct Fields
		{
			shim::LPARAM mXCoord : 16;
			shim::LPARAM mYCoord : 16;
			//shim::LPARAM Padding : 32; // Only on 64-bit
		} m_Fields;
	};
	static_assert( sizeof( MouseCoord::Fields ) == sizeof( shim::LPARAM ), "" );
	static_assert( sizeof( MouseCoord ) == sizeof( shim::LPARAM ), "" );

	switch( message )
	{
		// wParam: Modifiers
		// lParam [bit 0-15] : Absolute x-coord in client area
		// lParam [bit 16-31] : Absolute y-coord in client area
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEMOVE:
		{
			// NOTE: Signed, may go negative
			MouseCoord const mouseCoord( lParam );
			int16_t const xcoord = mouseCoord.m_Fields.mXCoord;
			int16_t const ycoord = mouseCoord.m_Fields.mYCoord;
			mCurrentSignalValues[k_CursorAbsoluteX] = xcoord;
			mCurrentSignalValues[k_CursorAbsoluteY] = ycoord;
			intercepted = true;
			return 0;
		}


		default:
		{
			intercepted = false;
			return 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void WndProcTextInputComponent::OnTick()
{
	// No action needed
}



void WndProcTextInputComponent::GetTextStream( rftl::u16string& text, size_t maxLen ) const
{
	text.clear();
	text.reserve( math::Min( maxLen, mTextBuffer.size() ) );
	for( char16_t const& ch : mTextBuffer )
	{
		text.push_back( ch );
		if( text.size() == maxLen )
		{
			break;
		}
	}
}



void WndProcTextInputComponent::ClearTextStream()
{
	mTextBuffer.clear();
}

///////////////////////////////////////////////////////////////////////////////

shim::LRESULT WndProcTextInputComponent::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted )
{
	switch( message )
	{
		// wParam: UTF16 Character code
		// lParam: Miscellaneous extra information (mostly WM_KEYDOWN bits)
		case WM_CHAR:
		{
			RF_ASSERT_MSG(
				win32::IsWindowUnicode( static_cast<win32::HWND>( hWnd ) ),
				"Non-unicode window, may not be able to receive unicode text properly" );
			char16_t const utf16 = static_cast<char16_t>( wParam );
			if( mTextBuffer.size() == kMaxStorage )
			{
				mTextBuffer.pop_front();
			}
			mTextBuffer.push_back( utf16 );
			intercepted = true;
			return 0;
		}


		default:
		{
			intercepted = false;
			return 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
