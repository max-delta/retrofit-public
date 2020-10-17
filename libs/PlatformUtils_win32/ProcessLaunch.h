#pragma once
#include "project.h"


namespace RF::platform::process {
///////////////////////////////////////////////////////////////////////////////

// Primarily intended for multi-client debug convenience, tries to launch a
//  copy of the current process in a reasonably clean manner, with minimal
//  interference from the currently running parent
PLATFORMUTILS_API bool LaunchSelfClone( bool autoClose );

///////////////////////////////////////////////////////////////////////////////
}
