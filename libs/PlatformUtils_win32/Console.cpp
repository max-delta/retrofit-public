#include "stdafx.h"
#include "Console.h"

#include "core_platform/inc/windows_inc.h"

#include "rftl/cstdio"


namespace RF::platform::console {
///////////////////////////////////////////////////////////////////////////////

bool EnableANSIEscapeSequences()
{
	win32::HANDLE const stdOutputHandle = win32::GetStdHandle( static_cast<win32::DWORD>( -11 ) /*STD_OUTPUT_HANDLE*/ );
	if( stdOutputHandle == reinterpret_cast<win32::HANDLE>( -1 ) /* INVALID_HANDLE_VALUE*/ )
	{
		return false;
	}

	win32::DWORD mode;
	win32::BOOL const modeGetSuccess = win32::GetConsoleMode( stdOutputHandle, &mode );
	if( modeGetSuccess == false )
	{
		return false;
	}

	// NOTE: At time of writing, there's a bug in the default Windows console
	//  that garbles any newline that hits the final column if ANSI terminal
	//  output is enabled
	// SEE: https://stackoverflow.com/questions/66244924/windows-console-conhost-discards-newline-when-output-matches-witdth-of-the-win
	static constexpr win32::DWORD kNeededBits =
		1 /*ENABLE_PROCESSED_OUTPUT*/ |
		4 /*ENABLE_VIRTUAL_TERMINAL_PROCESSING*/;

	if( ( mode & kNeededBits ) == kNeededBits )
	{
		// Already set
		return true;
	}

	win32::BOOL const modeSetSuccess = win32::SetConsoleMode(
		stdOutputHandle,
		mode | kNeededBits );
	if( modeSetSuccess == false )
	{
		return false;
	}

	return true;
}



bool IsAFullyOwnedConsoleWindow()
{
	// Get our window
	win32::HWND const consoleWindow = win32::GetConsoleWindow();
	if( consoleWindow == nullptr )
	{
		// No console window present
		return false;
	}

	// Who made it?
	win32::DWORD consoleProcessID = {};
	win32::DWORD const consoleCreatorThreadID =
		win32::GetWindowThreadProcessId( consoleWindow, &consoleProcessID );
	if( consoleCreatorThreadID == 0 )
	{
		RF_DBGFAIL_MSG( "According to documentation, this should not fail if the handle was valid" );
		return false;
	}

	// Was it us?
	win32::DWORD const currentProcessID = win32::GetCurrentProcessId();
	if( currentProcessID == consoleProcessID )
	{
		// We made it
		return true;
	}

	// Don't know who made it
	return false;
}



void TriggerPressAnyKeyOnProcessExit()
{
	static constexpr auto onExit = []() -> void
	{
		rftl::puts( "" );
		rftl::puts( "This seems to be a standalone console window, pausing on exit." );
		rftl::system( "pause" );
	};
	rftl::atexit( onExit );
}

///////////////////////////////////////////////////////////////////////////////
}
