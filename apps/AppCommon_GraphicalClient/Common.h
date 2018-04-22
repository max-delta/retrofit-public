#pragma once
#include "project.h"

#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PPU/PPUController.h"
#include "PlatformFilesystem/VFS.h"

namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

// Global systems
// TODO: Singleton manager
APPCOMMONGRAPHICALCLIENT_API extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;
APPCOMMONGRAPHICALCLIENT_API extern RF::UniquePtr<RF::gfx::PPUController> g_Graphics;
APPCOMMONGRAPHICALCLIENT_API extern RF::UniquePtr<RF::file::VFS> g_Vfs;

APPCOMMONGRAPHICALCLIENT_API void Startup();
APPCOMMONGRAPHICALCLIENT_API void Shutdown();

///////////////////////////////////////////////////////////////////////////////
}}
