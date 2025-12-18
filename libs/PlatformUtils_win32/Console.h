#pragma once
#include "project.h"


namespace RF::platform::console {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API bool EnableANSIEscapeSequences();

PLATFORMUTILS_API bool IsAFullyOwnedConsoleWindow();

PLATFORMUTILS_API void TriggerPressAnyKeyOnProcessExit();

///////////////////////////////////////////////////////////////////////////////
}
