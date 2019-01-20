#include "stdafx.h"
#include "Common.h"

#include "AppCommon_GraphicalClient/StandardTaskScheduler.h"

#include "PPU/PPUController.h"
#include "SimpleGL/SimpleGL.h"

#include "PlatformUtils_win32/Console.h"
#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformFilesystem/VFS.h"
#include "Logging/ANSIConsoleLogger.h"
#include "Logging/AssertLogger.h"

#include "core_math/math_bits.h"
#include "core/ptr/default_creator.h"

#include "rftl/thread"


namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );

// Global systems
APPCOMMONGRAPHICALCLIENT_API WeakPtr<input::WndProcInputDevice> gWndProcInput;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<gfx::PPUController> gGraphics;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<file::VFS> gVfs;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<app::StandardTaskScheduler> gTaskScheduler;
static UniquePtr<input::WndProcInputDevice> sWndProcInput;
static UniquePtr<gfx::PPUController> sGraphics;
static UniquePtr<file::VFS> sVfs;
static UniquePtr<app::StandardTaskScheduler> sTaskScheduler;

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Main start" );

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
		RF_DBGFAIL_MSG( "TODO: Non-ANSI console logger" );
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
	bool const vfsInitialized = gVfs->AttemptInitialMount( "../../config/vfs_game.ini", "../../../rftest_user" );
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

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing task manager..." );
	sTaskScheduler = DefaultCreator<app::StandardTaskScheduler>::Create( rftl::thread::hardware_concurrency() - 1 );
	gTaskScheduler = sTaskScheduler;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Startup complete" );
}



void Shutdown()
{
	sGraphics = nullptr;
	sVfs = nullptr;
	sWndProcInput = nullptr;
	sTaskScheduler = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}
