#include "stdafx.h"
#include "project.h"

#include "core_platform/ThreadSettings.h"

#include "core_platform/inc/windows_inc.h"

#include "rftl/string"
#include "rftl/string_view"


namespace RF::platform::thread {
///////////////////////////////////////////////////////////////////////////////

void SetThreadName( char const* label )
{
	rftl::string_view const ascii{ label };
	rftl::wstring const naive{ ascii.begin(), ascii.end() };
	win32::SetThreadDescription( win32::GetCurrentThread(), naive.c_str() );
}



void SetThreadPriority( ThreadPriority priority )
{
	switch( priority )
	{
		case ThreadPriority::High:
			win32::SetThreadPriority( win32::GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );
			break;
		case ThreadPriority::Normal:
			win32::SetThreadPriority( win32::GetCurrentThread(), THREAD_PRIORITY_NORMAL );
			break;
		case ThreadPriority::Low:
			win32::SetThreadPriority( win32::GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL );
			break;
		case ThreadPriority::Invalid:
		default:
			RF_DBGFAIL();
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
}
