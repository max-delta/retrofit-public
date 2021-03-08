#pragma once
#include "project.h"

#include "core_platform/shim/win_shim.h"

#include "rftl/cstdint"


namespace RF::platform::windowing {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API shim::HWND CreateNewWindow( int width = 640, int height = 480, shim::WNDPROC WndProc = shim::DefWindowProcW );

// Positive: Number of messages processed
// Zero: No processing needed
// Negative: Should quit the application
PLATFORMUTILS_API int32_t ProcessAllMessages();
PLATFORMUTILS_API int32_t ProcessSingleMessage();

///////////////////////////////////////////////////////////////////////////////
}
