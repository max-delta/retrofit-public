#include "stdafx.h"
#include "Console.h"

#include "core_platform/inc/windows_inc.h"


namespace RF::platform::console {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API bool EnableANSIEscapeSequences()
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

///////////////////////////////////////////////////////////////////////////////
}
