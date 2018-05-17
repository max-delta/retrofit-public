#include "stdafx.h"

#include "AppCommon_GraphicalClient/Common.h"
#include "Tests.h"

#include "PPU/PPUController.h"

#include "PlatformUtils_win32/windowing.h"
#include "PlatformUtils_win32/Console.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformFilesystem/VFS.h"
#include "SimpleGL/SimpleGL.h"
#include "Logging/Logging.h"
#include "Logging/ANSIConsoleLogger.h"
#include "Timing/clocks.h"
#include "Timing/Limiter.h"

#include "core_math/math_bits.h"
#include "core/ptr/default_creator.h"
#include "core/ptr/entwined_creator.h"

#include "rftl/thread"


///////////////////////////////////////////////////////////////////////////////

constexpr bool k_ConsoleTest = true;
constexpr bool k_DrawTest = true;
constexpr bool k_DrawInputDebug = true;
constexpr bool k_SquirrelTest = true;
constexpr bool k_XMLTest = true;
constexpr bool k_FPackSerializationTest = false;
constexpr bool k_PlatformTest = true;
constexpr bool k_FrameBuilderTest = true;

///////////////////////////////////////////////////////////////////////////////

int main()
{
	using namespace RF;

	app::Startup();

	if( k_ConsoleTest )
	{
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_TRACE, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_DEBUG, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_INFO, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_WARNING, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_ERROR, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_CRITICAL, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, logging::RF_SEV_MILESTONE, "Console test" );
		RFLOG_CUSTOM( nullptr, RFCAT_STARTUP, 1ull << 32, "Console test" );
	}

	if( k_SquirrelTest )
	{
		test::SQTest();
	}

	if( k_XMLTest )
	{
		test::XMLTest();
	}

	if( k_PlatformTest )
	{
		test::PlatformTest();
	}

	if( k_FPackSerializationTest )
	{
		test::FPackSerializationTest();
	}

	if( k_DrawTest )
	{
		test::InitDrawTest();
	}

	if( k_FrameBuilderTest )
	{
		test::InitFrameBuilderTest();
	}

	time::Limiter<rftl::chrono::nanoseconds, 16666666> frameLimiter;
	frameLimiter.Reset();

	while( true )
	{
		frameLimiter.Stall();

		app::g_WndProcInput->OnTick();

		if( platform::windowing::ProcessAllMessages() < 0 )
		{
			break;
		}

		app::g_Graphics->BeginFrame();
		{
			if( k_DrawInputDebug )
			{
				test::DrawInputDebug();
			}

			if( k_DrawTest )
			{
				test::DrawTest();
			}

			if( k_FrameBuilderTest )
			{
				test::FrameBuilderTest();
			}

			app::g_Graphics->SubmitToRender();
			app::g_Graphics->WaitForRender();
		}
		app::g_Graphics->EndFrame();
	}
	if( k_FrameBuilderTest )
	{
		test::TerminateFrameBuilderTest();
	}
	app::Shutdown();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
