#include "stdafx.h"

#include "cc3o3/cc3o3.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PlatformUtils_win32/windowing.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "Timing/Limiter.h"

///////////////////////////////////////////////////////////////////////////////

int main()
{
	using namespace RF;

	app::Startup();
	cc::Startup();

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
