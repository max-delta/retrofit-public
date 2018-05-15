#pragma once
#include "project.h"

#include "core/ptr/ptr_fwd.h"

// Forwards
namespace RF {
namespace input {
	class WndProcInputDevice;
}
namespace gfx {
	class PPUController;
}
namespace file {
	class VFS;
}
namespace app {
	class StandardTaskScheduler;
}
}

namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

// Global systems
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<input::WndProcInputDevice> g_WndProcInput;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<gfx::PPUController> g_Graphics;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<file::VFS> g_Vfs;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<app::StandardTaskScheduler> g_TaskScheduler;

APPCOMMONGRAPHICALCLIENT_API void Startup();
APPCOMMONGRAPHICALCLIENT_API void Shutdown();

///////////////////////////////////////////////////////////////////////////////
}}
