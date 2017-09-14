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

#include <thread>

///////////////////////////////////////////////////////////////////////////////

// Window Procedure in different file
shim::LRESULT WIN32_CALLBACK WndProc( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam );
extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;

// Global systems
// TODO: Singleton manager
RF::UniquePtr<RF::gfx::PPUController> g_Graphics;
RF::UniquePtr<RF::file::VFS> g_Vfs;

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

	if( test::squirrelTest )
	{
		test::SQTest();
	}

	shim::HWND hwnd = platform::windowing::CreateNewWindow( 640, 480, WndProc );
	UniquePtr<gfx::DeviceInterface> renderDevice = EntwinedCreator<gfx::SimpleGL>::Create();
	renderDevice->AttachToWindow( hwnd );

	g_Graphics = DefaultCreator<gfx::PPUController>::Create( std::move( renderDevice ) );
	g_Graphics->Initialize( 640, 480 );

	g_WndProcInput = EntwinedCreator<input::WndProcInputDevice>::Create();

	if( test::drawTest )
	{
		test::InitDrawTest();
	}

	FrameLimiter frameLimiter;
	frameLimiter.Reset();
	while( platform::windowing::ProcessAllMessages() >= 0 )
	{
		frameLimiter.Stall();

		g_Graphics->BeginFrame();
		{
			g_WndProcInput->OnTick();

			if( test::drawInputDebug )
			{
				test::DrawInputDebug();
			}

			if( test::drawTest )
			{
				test::DrawTest();
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
