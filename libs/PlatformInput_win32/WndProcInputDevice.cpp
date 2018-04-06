#include "stdafx.h"
#include "WndProcInputDevice.h"

#include "core_platform/windows_inc.h"
#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

WndProcInputDevice::WndProcInputDevice()
	: InputDevice(
		DefaultCreator<WndProcDigitalInputComponent>::Create(),
		EntwinedCreator<WndProcAnalogInputComponent>::Create(),
		EntwinedCreator<WndProcTextInputComponent>::Create() )
	, m_Digital( *(WndProcDigitalInputComponent*)m_DigitalComponent.Get() )
	, m_Analog( *(WndProcAnalogInputComponent*)m_AnalogComponent.Get() )
	, m_Text( *(WndProcTextInputComponent*)m_TextComponent.Get() )
{
	//
}



shim::LRESULT WndProcInputDevice::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool & intercepted )
{
	shim::LRESULT retVal = {};
	shim::LRESULT componentResult;
	bool componentIntercepted = false;

	componentResult = m_Digital.ExamineTranslatedMessage( hWnd, message, wParam, lParam, componentIntercepted );
	if( componentIntercepted )
	{
		retVal = componentResult;
		intercepted = true;
	}

	componentResult = m_Analog.ExamineTranslatedMessage( hWnd, message, wParam, lParam, componentIntercepted );
	if( componentIntercepted )
	{
		retVal = componentResult;
		intercepted = true;
	}

	componentResult = m_Text.ExamineTranslatedMessage( hWnd, message, wParam, lParam, componentIntercepted );
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
	m_PreviousLogicalState = m_CurrentLogicalState;
	m_PreviousPhysicalState= m_CurrentPhysicalState;
}



WndProcDigitalInputComponent::PhysicalCode WndProcDigitalInputComponent::GetMaxPhysicalCode() const
{
	RF_DBGFAIL_MSG( "TODO" );
	return PhysicalCode();
}



WndProcDigitalInputComponent::LogicalCode WndProcDigitalInputComponent::GetMaxLogicalCode() const
{
	RF_DBGFAIL_MSG( "TODO" );
	return LogicalCode();
}



std::u16string WndProcDigitalInputComponent::GetLogicalName( LogicalCode code ) const
{
	RF_DBGFAIL_MSG( "TODO" );
	return std::u16string();
}



WndProcDigitalInputComponent::PinState WndProcDigitalInputComponent::GetCurrentPhysicalState( PhysicalCode code ) const
{
	return m_CurrentPhysicalState[code] ? PinState::Active : PinState::Inactive;
}



WndProcDigitalInputComponent::PinState WndProcDigitalInputComponent::GetPreviousPhysicalState( PhysicalCode code ) const
{
	return m_PreviousPhysicalState[code] ? PinState::Active : PinState::Inactive;
}



WndProcDigitalInputComponent::PinState WndProcDigitalInputComponent::GetCurrentLogicalState( LogicalCode code ) const
{
	return m_CurrentLogicalState[code] ? PinState::Active : PinState::Inactive;
}



WndProcDigitalInputComponent::PinState WndProcDigitalInputComponent::GetPreviousLogicalState( LogicalCode code ) const
{
	return m_PreviousLogicalState[code] ? PinState::Active : PinState::Inactive;
}



void WndProcDigitalInputComponent::GetPhysicalEventStream( EventParser<PhysicalEvent>& parser, size_t maxEvents ) const
{
	size_t const numToRead = math::Min( m_PhysicalEventBuffer.size(), maxEvents );
	for( size_t i = m_PhysicalEventBuffer.size() - numToRead; i < m_PhysicalEventBuffer.size(); i++ )
	{
		parser.OnEvent( m_PhysicalEventBuffer[i] );
	}
}



void WndProcDigitalInputComponent::GetLogicalEventStream( EventParser<LogicalEvent>& parser, size_t maxEvents ) const
{
	size_t const numToRead = math::Min( m_LogicalEventBuffer.size(), maxEvents );
	for( size_t i = m_LogicalEventBuffer.size() - numToRead; i < m_LogicalEventBuffer.size(); i++ )
	{
		parser.OnEvent( m_LogicalEventBuffer[i] );
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

shim::LRESULT WndProcDigitalInputComponent::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool & intercepted )
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
			PinState state;
			switch( message )
			{
				case WM_KEYDOWN:
					state = PinState::Active;
					break;
				case WM_KEYUP:
					state = PinState::Inactive;
					break;
				default:
					RF_DBGFAIL();
					state = PinState::Inactive;
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
			PinState state;
			switch( message )
			{
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_XBUTTONDOWN:
					state = PinState::Active;
					break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
				case WM_MBUTTONUP:
				case WM_XBUTTONUP:
					// NOTE: User can drag-escape window to hide this event
					state = PinState::Inactive;
					break;
				default:
					RF_DBGFAIL();
					state = PinState::Inactive;
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


		default:
		{
			intercepted = false;
			return 0;
		}
	}
}



void WndProcDigitalInputComponent::RecordLogical( uint8_t code, PinState state )
{
	if( m_CurrentLogicalState[code] == ( state == PinState::Active ) )
	{
		// Repeat, ignore
		return;
	}

	m_CurrentLogicalState[code] = state == PinState::Active;
	m_LogicalEventBuffer.emplace_back( code, state );
	while( m_LogicalEventBuffer.size() > k_MaxEventStorage )
	{
		m_LogicalEventBuffer.pop_front();
	}
}



void WndProcDigitalInputComponent::RecordPhysical( uint8_t code, PinState state )
{
	if( m_CurrentPhysicalState[code] == ( state == PinState::Active ) )
	{
		// Repeat, ignore
		return;
	}

	m_CurrentPhysicalState[code] = state == PinState::Active;
	m_PhysicalEventBuffer.emplace_back( code, state );
	while( m_PhysicalEventBuffer.size() > k_MaxEventStorage )
	{
		m_PhysicalEventBuffer.pop_front();
	}
}

///////////////////////////////////////////////////////////////////////////////

void WndProcAnalogInputComponent::OnTick()
{
	static_assert( std::is_array<SignalValues>::value, "Type changed, re-evalute memcpy safety" );
	memcpy( m_PreviousSignalValues, m_CurrentSignalValues, sizeof(SignalValues) );
}



WndProcAnalogInputComponent::SignalIndex WndProcAnalogInputComponent::GetMaxSignalIndex() const
{
	return k_NumSignals;
}



std::u16string WndProcAnalogInputComponent::GetSignalName( SignalIndex signalIndex ) const
{
	RF_DBGFAIL_MSG( "TODO" );
	return std::u16string();
}



WndProcAnalogInputComponent::SignalValue WndProcAnalogInputComponent::GetCurrentSignalValue( SignalIndex signalIndex ) const
{
	RF_ASSERT_MSG( signalIndex < GetMaxSignalIndex(), "Invalid parameter" );
	return m_CurrentSignalValues[signalIndex];
}



WndProcAnalogInputComponent::SignalValue WndProcAnalogInputComponent::GetPreviousSignalValue( SignalIndex signalIndex ) const
{
	RF_ASSERT_MSG( signalIndex < GetMaxSignalIndex(), "Invalid parameter" );
	return m_PreviousSignalValues[signalIndex];
}

///////////////////////////////////////////////////////////////////////////////

shim::LRESULT WndProcAnalogInputComponent::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool & intercepted )
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
			shim::LPARAM m_XCoord : 16;
			shim::LPARAM m_YCoord : 16;
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
			int16_t const xcoord = mouseCoord.m_Fields.m_XCoord;
			int16_t const ycoord = mouseCoord.m_Fields.m_YCoord;
			m_CurrentSignalValues[k_CursorAbsoluteX] = xcoord;
			m_CurrentSignalValues[k_CursorAbsoluteY] = ycoord;
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



void WndProcTextInputComponent::GetTextStream( std::u16string & text, size_t maxLen ) const
{
	text.clear();
	text.reserve( maxLen );
	for( char16_t const& ch : m_TextBuffer )
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
	m_TextBuffer.clear();
}

///////////////////////////////////////////////////////////////////////////////

shim::LRESULT WndProcTextInputComponent::ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool & intercepted )
{
	switch( message )
	{
		// wParam: UTF16 Character code
		// lParam: Miscellaneous extra information (mostly WM_KEYDOWN bits)
		case WM_CHAR:
		{
			RF_ASSERT_MSG(
				win32::IsWindowUnicode( ( win32::HWND )hWnd ),
				"Non-unicode window, may not be able to receive unicode text properly" );
			char16_t const utf16 = (char16_t)wParam;
			if( m_TextBuffer.size() == k_MaxStorage )
			{
				m_TextBuffer.pop_front();
			}
			m_TextBuffer.push_back( utf16 );
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
