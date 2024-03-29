#pragma once
#include "project.h"

#include "core/ptr/ptr_fwd.h"

// Forwards
namespace RF {
namespace cli {
	class ArgView;
	class ArgParse;
}
namespace input {
	class ControllerManager;
	class WndProcInputDevice;
}
namespace gfx::ppu {
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
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<cli::ArgParse const> gCommandLineArgs;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<input::ControllerManager> gInputControllerManager;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<input::WndProcInputDevice> gWndProcInput;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<gfx::ppu::PPUController> gGraphics;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<ui::FontRegistry> gFontRegistry;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<ui::ContainerManager> gUiManager;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<loc::LocEngine> gLocEngine;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<loc::PageMapper> gPageMapper;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<file::VFS> gVfs;
APPCOMMONGRAPHICALCLIENT_API extern WeakPtr<app::StandardTaskScheduler> gTaskScheduler;

APPCOMMONGRAPHICALCLIENT_API void Startup( cli::ArgView const& args );
APPCOMMONGRAPHICALCLIENT_API void Shutdown();

///////////////////////////////////////////////////////////////////////////////
}
