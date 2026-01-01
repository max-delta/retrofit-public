#include "stdafx.h"

#include "cc3o3/cc3o3.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/sync/Session.h"
#include "cc3o3/time/TimeFwd.h"

#include "AppCommon_GraphicalClient/Common.h"
#include "AppCommon_GraphicalClient/StartupConfig.h"

#include "PPU/PPUController.h"
#include "PlatformUtils_win32/windowing.h"
#include "CommandLine/ArgView.h"
#include "Timing/FrameClock.h"
#include "Timing/Limiter.h"

///////////////////////////////////////////////////////////////////////////////

RF_MODULE_POINT int module_main( int argc, char* argv[] )
{
	using namespace RF;

	// Startup
	app::Startup( { argc, argv }, app::StartupConfig{} );
	cc::Startup();

	using Limiter = cc::time::FrameLimiter;
	Limiter frameLimiter;
	frameLimiter.Reset();
	auto const stall = [&frameLimiter]() -> void //
	{
		using Speed = cc::sync::RecommendedFrameSpeed;
		Speed const frameSpeed = cc::sync::CalcRecommendedFrameSpeed();

		static constexpr rftl::chrono::microseconds kSpeedAdjustment( 1000 );
		time::CommonClock::duration adjustment = {};
		if( frameSpeed == Speed::SpeedUp )
		{
			adjustment = -kSpeedAdjustment;
		}
		else if( frameSpeed == Speed ::SlowDown )
		{
			adjustment = kSpeedAdjustment;
		}

		frameLimiter.AdjustedStall( adjustment );
	};

	while( true )
	{
		// Timing
		stall();
		time::FrameClock::add_time( cc::time::kSimulationFrameDuration );

		// Input
		cc::input::HardcodedDeviceTick();

		// Windows
		if( platform::windowing::ProcessAllMessages() < 0 )
		{
			break;
		}

		// Exit
		if( app::gShouldExit )
		{
			break;
		}

		// Frame
		app::gGraphics->BeginFrame();
		{
			cc::ProcessFrame();

			app::gGraphics->SubmitToRender();
			app::gGraphics->WaitForRender();
		}
		app::gGraphics->EndFrame();
	}

	// Shutdown
	cc::Shutdown();
	app::Shutdown();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
