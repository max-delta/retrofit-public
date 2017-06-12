#pragma once
#include "project.h"

#include "core_platform/win_shim.h"

namespace RF { namespace platform { namespace windowing {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API shim::HWND CreateNewWindow( int width = 640, int height = 480, shim::WNDPROC WndProc = shim::DefWindowProcW );

PLATFORMUTILS_API bool ProcessMessages();

///////////////////////////////////////////////////////////////////////////////
}}}
