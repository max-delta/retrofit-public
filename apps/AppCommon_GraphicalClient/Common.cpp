#include "stdafx.h"
#include "Common.h"

#include "AppCommon_GraphicalClient/StandardTaskScheduler.h"

#include "GameInput/ControllerManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"
#include "SimpleGL/SimpleGL.h"

#include "CommandLine/ArgParse.h"
#include "CommandLine/ArgView.h"
#include "Localization/LocEngine.h"
#include "Localization/PageMapper.h"
#include "Logging/ANSIConsoleLogger.h"
#include "Logging/AssertLogger.h"
#include "Rollback/RollbackManager.h"

#include "PlatformUtils_win32/Console.h"
#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformFilesystem/VFS.h"

#include "core_math/math_bits.h"
#include "core/ptr/default_creator.h"

#include "rftl/thread"


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );

APPCOMMONGRAPHICALCLIENT_API bool gShouldExit = false;

// Global systems
APPCOMMONGRAPHICALCLIENT_API WeakPtr<cli::ArgParse const> gCommandLineArgs;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<input::WndProcInputDevice> gWndProcInput;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<input::ControllerManager> gInputControllerManager;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<gfx::PPUController> gGraphics;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<ui::FontRegistry> gFontRegistry;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<ui::ContainerManager> gUiManager;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<loc::LocEngine> gLocEngine;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<loc::PageMapper> gPageMapper;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<file::VFS> gVfs;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<app::StandardTaskScheduler> gTaskScheduler;
static UniquePtr<cli::ArgParse const> sCommandLineArgs;
static UniquePtr<input::WndProcInputDevice> sWndProcInput;
static UniquePtr<input::ControllerManager> sInputControllerManager;
static UniquePtr<gfx::PPUController> sGraphics;
static UniquePtr<ui::FontRegistry> sFontRegistry;
static UniquePtr<ui::ContainerManager> sUiManager;
static UniquePtr<loc::LocEngine> sLocEngine;
static UniquePtr<loc::PageMapper> sPageMapper;
static UniquePtr<file::VFS> sVfs;
static UniquePtr<app::StandardTaskScheduler> sTaskScheduler;

///////////////////////////////////////////////////////////////////////////////

void Startup( cli::ArgView const& args )
{
	sCommandLineArgs = DefaultCreator<cli::ArgParse>::Create( args );
	gCommandLineArgs = sCommandLineArgs;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Main startup" );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing console logging..." );
	bool const consoleInitialized = platform::console::EnableANSIEscapeSequences();
	if( consoleInitialized )
	{
		puts( " == \x1b[1;32mANSI CONSOLE SUPPORT\x1b[0m ==" );
		logging::HandlerDefinition def;
		def.mSupportedSeverities = math::GetAllBitsSet<logging::SeverityMask>();
		def.mUtf8HandlerFunc = logging::ANSIConsoleLogger;
		logging::RegisterHandler( def );
	}
	else
	{
		puts( " == NO ANSI CONSOLE SUPPORT ==" );
		RF_TODO_BREAK_MSG( "Non-ANSI console logger" );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing assert logging..." );
	{
		logging::HandlerDefinition def;
		def.mSupportedSeverities = logging::Severity::RF_SEV_USER_ATTENTION_REQUESTED;
		def.mUtf8HandlerFunc = logging::AssertLogger;
		logging::RegisterHandler( def );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing VFS..." );
	sVfs = DefaultCreator<file::VFS>::Create();
	gVfs = sVfs;
	static constexpr file::MountPriority kMountPriority = file::kMountPriorityHighest;
	bool vfsInitialized = gVfs->AttemptInitialMount( kMountPriority, "config/vfs_core.ini", "_user" );
	if( vfsInitialized == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_STARTUP, "Failed to startup VFS, trying again assuming _binaries launch" );
		vfsInitialized = gVfs->AttemptInitialMount( kMountPriority, "../config/vfs_core.ini", "../_user" );
	}
	if( vfsInitialized == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_STARTUP, "Failed to startup VFS, trying again assuming F5 launch" );
		vfsInitialized = gVfs->AttemptInitialMount( kMountPriority, "../../config/vfs_core.ini", "../../_user" );
	}
	if( vfsInitialized == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_STARTUP, "Failed to startup VFS, trying again assuming _artifacts launch" );
		vfsInitialized = gVfs->AttemptInitialMount( kMountPriority, "../../../../config/vfs_core.ini", "../../../../_user" );
	}
	if( vfsInitialized == false )
	{
		RFLOG_FATAL( nullptr, RFCAT_STARTUP, "Failed to startup VFS" );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Creating window..." );
	constexpr uint8_t k_WindowScaleFactor = 4;
	constexpr uint16_t k_Width = gfx::kDesiredWidth * k_WindowScaleFactor;
	constexpr uint16_t k_Height = gfx::kDesiredHeight * k_WindowScaleFactor;
	shim::HWND hwnd = platform::windowing::CreateNewWindow( k_Width, k_Height, WndProc );
	UniquePtr<gfx::DeviceInterface> renderDevice = DefaultCreator<gfx::SimpleGL>::Create();
	renderDevice->AttachToWindow( hwnd );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing graphics..." );
	sGraphics = DefaultCreator<gfx::PPUController>::Create( rftl::move( renderDevice ), gVfs );
	gGraphics = sGraphics;
	gGraphics->Initialize( k_Width, k_Height );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing input..." );
	sWndProcInput = DefaultCreator<input::WndProcInputDevice>::Create();
	gWndProcInput = sWndProcInput;
	sInputControllerManager = DefaultCreator<input::ControllerManager>::Create();
	gInputControllerManager = sInputControllerManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing font registry..." );
	sFontRegistry = DefaultCreator<ui::FontRegistry>::Create();
	gFontRegistry = sFontRegistry;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing ui..." );
	sUiManager = DefaultCreator<ui::ContainerManager>::Create( gGraphics, gFontRegistry );
	gUiManager = sUiManager;
	gUiManager->CreateRootContainer();

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing localization..." );
	sLocEngine = DefaultCreator<loc::LocEngine>::Create();
	gLocEngine = sLocEngine;
	sPageMapper = DefaultCreator<loc::PageMapper>::Create();
	gPageMapper = sPageMapper;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing task manager..." );
	sTaskScheduler = DefaultCreator<app::StandardTaskScheduler>::Create( rftl::thread::hardware_concurrency() - 1 );
	gTaskScheduler = sTaskScheduler;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Main startup complete" );
}



void Shutdown()
{
	sUiManager = nullptr;
	sFontRegistry = nullptr;
	sGraphics = nullptr;
	sVfs = nullptr;
	sWndProcInput = nullptr;
	sTaskScheduler = nullptr;
	sCommandLineArgs = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}
