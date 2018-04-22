#include "stdafx.h"
#include "Common.h"

#include "PPU/PPUController.h"
#include "SimpleGL/SimpleGL.h"

#include "PlatformUtils_win32/Console.h"
#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformFilesystem/VFS.h"
#include "Logging/ANSIConsoleLogger.h"

#include "core_math/math_bits.h"
#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"

namespace RF { namespace app {
///////////////////////////////////////////////////////////////////////////////

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );

// Global systems
APPCOMMONGRAPHICALCLIENT_API WeakPtr<input::WndProcInputDevice> g_WndProcInput;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<gfx::PPUController> g_Graphics;
APPCOMMONGRAPHICALCLIENT_API WeakPtr<file::VFS> g_Vfs;
static UniquePtr<input::WndProcInputDevice> s_WndProcInput;
static UniquePtr<gfx::PPUController> s_Graphics;
static UniquePtr<file::VFS> s_Vfs;

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
		def.mHandlerFunc = logging::ANSIConsoleLogger;
		logging::RegisterHandler( def );
	}
	else
	{
		puts( " == NO ANSI CONSOLE SUPPORT ==" );
		RF_DBGFAIL_MSG( "TODO: Non-ANSI console logger" );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing VFS..." );
	s_Vfs = DefaultCreator<file::VFS>::Create();
	g_Vfs = s_Vfs;
	bool const vfsInitialized = g_Vfs->AttemptInitialMount( "../../config/vfs_game.ini", "../../../rftest_user" );
	if( vfsInitialized == false )
	{
		RFLOG_FATAL( nullptr, RFCAT_STARTUP, "Failed to startup VFS" );
	}
	file::VFS::HACK_SetInstance( g_Vfs );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Creating window..." );
	constexpr uint8_t k_WindowScaleFactor = 4;
	constexpr uint16_t k_Width = gfx::k_DesiredWidth * k_WindowScaleFactor;
	constexpr uint16_t k_Height = gfx::k_DesiredHeight * k_WindowScaleFactor;
	shim::HWND hwnd = platform::windowing::CreateNewWindow( k_Width, k_Height, WndProc );
	UniquePtr<gfx::DeviceInterface> renderDevice = DefaultCreator<gfx::SimpleGL>::Create();
	renderDevice->AttachToWindow( hwnd );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing graphics..." );
	s_Graphics = DefaultCreator<gfx::PPUController>::Create( rftl::move( renderDevice ) );
	g_Graphics = s_Graphics;
	g_Graphics->Initialize( k_Width, k_Height );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing input..." );
	s_WndProcInput = EntwinedCreator<input::WndProcInputDevice>::Create();
	g_WndProcInput = s_WndProcInput;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Startup complete" );
}



void Shutdown()
{
	g_Graphics = nullptr;
	g_Vfs = nullptr;
	g_WndProcInput = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}
