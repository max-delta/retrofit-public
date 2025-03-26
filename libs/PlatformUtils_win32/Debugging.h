#pragma once
#include "project.h"


namespace RF::platform::debugging {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API void OutputToDebugger( char const* string );

// NOTE: Emphasis on 'obviously', as debuggers can hide themselves
// NOTE: Doesn't check for obvious kernel debugger activity
PLATFORMUTILS_API bool IsUserModeDebuggerObviouslyAttached();

// NOTE: Emphasis on 'obviously', as debuggers can hide themselves
// NOTE: User-mode debuggers won't show up here
PLATFORMUTILS_API bool IsKernelModeDebuggerObviouslyAttached();

///////////////////////////////////////////////////////////////////////////////
}
