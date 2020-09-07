#include "stdafx.h"
#include "project.h"

#include "AppCommon_GraphicalClient/Common.h"
#include "Tests.h"
#include "Scratch.h"

#include "GameUI/ContainerManager.h"

#include "PPU/PPUController.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformUtils_win32/windowing.h"
#include "Logging/Logging.h"
#include "Timing/Limiter.h"

///////////////////////////////////////////////////////////////////////////////

constexpr bool kAllowTests = true;

constexpr bool kConsoleTest = true;
constexpr bool kDrawTest = true;
constexpr bool kInputDebug = true;
constexpr bool kUITest = true;
constexpr bool kXMLTest = true;
constexpr bool kFPackSerializationTest = true;
constexpr bool kPlatformTest = true;
constexpr bool kFrameBuilderTest = true;
constexpr bool kSQReflectTest = true;
constexpr bool kTCPTest = true;

constexpr bool kAllowScratch = true;

constexpr bool kDebugUI = true;

///////////////////////////////////////////////////////////////////////////////

void TestStart()
{
	using namespace RF;

	if( kConsoleTest )
	{
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_TRACE, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_DEBUG, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_INFO, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_WARNING, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_ERROR, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_CRITICAL, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_MILESTONE, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, 1ull << 32, "Console test" );

		// TODO
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_INFO, u"Utf16 test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_INFO, U"Utf32 test" );
	}

	if( kXMLTest )
	{
		test::XMLTest();
	}

	if( kPlatformTest )
	{
		test::PlatformTest();
	}

	if( kFPackSerializationTest )
	{
		test::FPackSerializationTest();
	}

	if( kInputDebug )
	{
		test::InitInputDebug();
	}

	if( kDrawTest )
	{
		test::InitDrawTest();
	}

	if( kUITest )
	{
		test::InitUITest();
	}

	if( kFrameBuilderTest )
	{
		test::InitFrameBuilderTest();
	}

	if( kSQReflectTest )
	{
		test::SQReflectTest();
	}

	if( kTCPTest )
	{
		test::TCPTest();
	}
}



void TestRun()
{
	using namespace RF;

	if( kInputDebug )
	{
		test::DrawInputDebug();
	}

	if( kDrawTest )
	{
		test::DrawTest();
	}

	if( kFrameBuilderTest )
	{
		test::FrameBuilderTest();
	}
}



void TestEnd()
{
	using namespace RF;

	if( kFrameBuilderTest )
	{
		test::TerminateFrameBuilderTest();
	}
}

///////////////////////////////////////////////////////////////////////////////

RF_MODULE_POINT int module_main()
{
	using namespace RF;

	app::Startup();

	file::VFSPath const testDataMountFile = file::VFS::kRoot.GetChild( "config", "vfs_test.ini" );
	bool vfsTestDataLoad = app::gVfs->AttemptSubsequentMount( testDataMountFile );
	if( vfsTestDataLoad == false )
	{
		RFLOG_FATAL( testDataMountFile, RFCAT_STARTUP, "Can't load test data mount file" );
	}

	if( kAllowTests )
	{
		TestStart();
	}

	if( kAllowScratch )
	{
		scratch::Start();
	}

	using Limiter = time::Limiter60Fps;
	Limiter frameLimiter;
	frameLimiter.Reset();

	while( true )
	{
		frameLimiter.Stall();
		time::FrameClock::add_time( Limiter::kSpanDuration );

		app::gWndProcInput->OnTick();

		if( platform::windowing::ProcessAllMessages() < 0 )
		{
			break;
		}

		if( app::gShouldExit )
		{
			break;
		}

		app::gGraphics->BeginFrame();
		{
			if( kAllowTests )
			{
				TestRun();
			}

			if( kAllowScratch )
			{
				scratch::Run();
			}

			app::gUiManager->RecalcRootContainer();
			app::gUiManager->ProcessRecalcs();
			if( kDebugUI )
			{
				app::gUiManager->DebugRender( false, true, true );
			}
			app::gUiManager->Render();

			app::gGraphics->SubmitToRender();
			app::gGraphics->WaitForRender();
		}
		app::gGraphics->EndFrame();
	}

	if( kAllowTests )
	{
		TestEnd();
	}

	if( kAllowScratch )
	{
		scratch::End();
	}

	app::Shutdown();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
