#include "stdafx.h"

#include "PPU/PPUController.h"

#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformFilesystem/VFS.h"
#include "SimpleGL/SimpleGL.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"
#include "core_time/clocks.h"

#include "Tests.h"
#include "FramePackEditor.h"

#include <thread>

///////////////////////////////////////////////////////////////////////////////

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );
extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;

// Global systems
// TODO: Singleton manager
RF::UniquePtr<RF::gfx::PPUController> g_Graphics;
RF::UniquePtr<RF::file::VFS> g_Vfs;

constexpr bool k_DrawTest = false;
constexpr bool k_DrawInputDebug = false;
constexpr bool k_SquirrelTest = false;
constexpr bool k_FramePackEditor = true;
RF::UniquePtr<RF::FramePackEditor> g_FramePackEditor;

///////////////////////////////////////////////////////////////////////////////

struct FrameLimiter
{
	static RF::time::PerfClock::duration const k_DesiredFrameTime;
	void Reset()
	{
		m_FrameStart = RF::time::PerfClock::now();
		m_FrameEnd = RF::time::PerfClock::now();
	}
	void Stall()
	{
		using namespace RF;
		time::PerfClock::time_point const naturalFrameEnd = time::PerfClock::now();
		time::PerfClock::duration const naturalFrameTime = naturalFrameEnd - m_FrameStart;
		if( naturalFrameTime < k_DesiredFrameTime )
		{
			time::PerfClock::duration const timeRemaining = k_DesiredFrameTime - naturalFrameTime;
			std::this_thread::sleep_for( timeRemaining );
		}
		m_FrameEnd = time::PerfClock::now();
		time::PerfClock::duration const frameTime = m_FrameEnd - m_FrameStart;
		time::FrameClock::add_time( frameTime );
		m_FrameStart = time::PerfClock::now();
	}
	RF::time::PerfClock::time_point m_FrameStart;
	RF::time::PerfClock::time_point m_FrameEnd;
};
RF::time::PerfClock::duration const FrameLimiter::k_DesiredFrameTime = std::chrono::milliseconds( 33 );

///////////////////////////////////////////////////////////////////////////////

int main()
{
	using namespace RF;

	g_Vfs = DefaultCreator<file::VFS>::Create();
	bool const vfsInitialized = g_Vfs->AttemptInitialMount( "../../config/vfs_game.ini", "../../../rftest_user" );
	if( vfsInitialized == false )
	{
		RF_ASSERT_MSG( false, "Failed to startup VFS" );
		return 1;
	}
	file::VFS::HACK_SetInstance( g_Vfs );
	g_Vfs->DebugDumpMountTable();

	if( k_SquirrelTest )
	{
		test::SQTest();
	}

	constexpr uint8_t k_WindowScaleFactor = 4;
	constexpr uint16_t k_Width = gfx::k_DesiredWidth * k_WindowScaleFactor;
	constexpr uint16_t k_Height = gfx::k_DesiredHeight * k_WindowScaleFactor;

	shim::HWND hwnd = platform::windowing::CreateNewWindow( k_Width, k_Height, WndProc );
	UniquePtr<gfx::DeviceInterface> renderDevice = EntwinedCreator<gfx::SimpleGL>::Create();
	renderDevice->AttachToWindow( hwnd );

	g_Graphics = DefaultCreator<gfx::PPUController>::Create( std::move( renderDevice ) );
	g_Graphics->Initialize( k_Width, k_Height );

	g_WndProcInput = EntwinedCreator<input::WndProcInputDevice>::Create();

	if( k_DrawTest )
	{
		test::InitDrawTest();
	}

	if( k_FramePackEditor )
	{
		g_FramePackEditor = EntwinedCreator<FramePackEditor>::Create();
		g_FramePackEditor->Init();
	}

	FrameLimiter frameLimiter;
	frameLimiter.Reset();
	while( true )
	{
		frameLimiter.Stall();

		g_WndProcInput->OnTick();

		if( platform::windowing::ProcessAllMessages() < 0 )
		{
			break;
		}

		g_Graphics->BeginFrame();
		{
			if( k_DrawInputDebug )
			{
				test::DrawInputDebug();
			}

			if( k_DrawTest )
			{
				test::DrawTest();
			}

			if( k_FramePackEditor )
			{
				FramePackEditor* const framePackEditor = g_FramePackEditor;
				framePackEditor->Process();
				framePackEditor->Render();
			}

			g_Graphics->SubmitToRender();
			g_Graphics->WaitForRender();
		}
		g_Graphics->EndFrame();
	}
	g_Graphics = nullptr;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
