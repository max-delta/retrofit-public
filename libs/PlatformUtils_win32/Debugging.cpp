#include "stdafx.h"
#include "Debugging.h"

#include "core_platform/inc/windows_inc.h"


namespace RF { namespace platform { namespace debugging {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API void OutputToDebugger( char const* string )
{
	win32::OutputDebugStringA( string );
}

///////////////////////////////////////////////////////////////////////////////
}}}
