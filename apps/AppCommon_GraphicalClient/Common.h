#pragma once
#include "project.h"

#include "core/ptr/ptr_fwd.h"

// Forwards
namespace RF {
namespace input {
	class WndProcInputDevice;
	class ControllerManager;
}
namespace gfx {
	class PPUController;
}
namespace ui {
	class ContainerManager;
	class FontRegistry;
}
namespace loc {
	class LocEngine;
	class PageMapper;
}
namespace file {
	class VFS;
}
namespace app {
	class StandardTaskScheduler;
}
}

namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

APPCOMMONGRAPHICALCLIENT_API extern bool gShouldExit;

// Global systems
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<input::WndProcInputDevice> gWndProcInput;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<input::ControllerManager> gInputControllerManager;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<gfx::PPUController> gGraphics;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<ui::FontRegistry> gFontRegistry;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<ui::ContainerManager> gUiManager;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<loc::LocEngine> gLocEngine;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<loc::PageMapper> gPageMapper;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<file::VFS> gVfs;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<app::StandardTaskScheduler> gTaskScheduler;

APPCOMMONGRAPHICALCLIENT_API void Startup();
APPCOMMONGRAPHICALCLIENT_API void Shutdown();

///////////////////////////////////////////////////////////////////////////////
}
