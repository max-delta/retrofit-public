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

	win32::BOOL const modeSetSuccess = win32::SetConsoleMode(
		stdOutputHandle,
		mode | 4 /*ENABLE_VIRTUAL_TERMINAL_PROCESSING*/ );
	if( modeSetSuccess == false )
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
