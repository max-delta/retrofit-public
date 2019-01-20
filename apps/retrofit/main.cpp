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

#include "rftl/thread"


///////////////////////////////////////////////////////////////////////////////
namespace RF { namespace scratch {
void sqreflect_scratch();
}}
///////////////////////////////////////////////////////////////////////////////

constexpr bool kConsoleTest = true;
constexpr bool kDrawTest = true;
constexpr bool kDrawInputDebug = true;
constexpr bool kXMLTest = true;
constexpr bool kFPackSerializationTest = true;
constexpr bool kPlatformTest = true;
constexpr bool kFrameBuilderTest = true;
constexpr bool kSQReflectTest = true;

///////////////////////////////////////////////////////////////////////////////

int main()
{
	using namespace RF;

	app::Startup();

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

	if( kDrawTest )
	{
		test::InitDrawTest();
	}

	if( kFrameBuilderTest )
	{
		test::InitFrameBuilderTest();
	}

	if( kSQReflectTest )
	{
		scratch::sqreflect_scratch();
	}

	time::Limiter<rftl::chrono::nanoseconds, 16666666> frameLimiter;
	frameLimiter.Reset();

	while( true )
	{
		frameLimiter.Stall();

		app::gWndProcInput->OnTick();

		if( platform::windowing::ProcessAllMessages() < 0 )
		{
			break;
		}

		app::gGraphics->BeginFrame();
		{
			if( kDrawInputDebug )
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

			app::gGraphics->SubmitToRender();
			app::gGraphics->WaitForRender();
		}
		app::gGraphics->EndFrame();
	}
	if( kFrameBuilderTest )
	{
		test::TerminateFrameBuilderTest();
	}
	app::Shutdown();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
