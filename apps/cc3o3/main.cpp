#include "stdafx.h"

#include "cc3o3/cc3o3.h"
#include "cc3o3/time/TimeFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "Timing/Limiter.h"

///////////////////////////////////////////////////////////////////////////////

RF_MODULE_POINT int module_main()
{
	using namespace RF;

	app::Startup();
	cc::Startup();

	using Limiter = cc::time::FrameLimiter;
	Limiter frameLimiter;
	frameLimiter.Reset();

	while( true )
	{
		frameLimiter.Stall();
		time::FrameClock::add_time( cc::time::kSimulationFrameDuration );

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
			cc::ProcessFrame();

			app::gGraphics->SubmitToRender();
			app::gGraphics->WaitForRender();
		}
		app::gGraphics->EndFrame();
	}

	cc::Shutdown();
	app::Shutdown();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
