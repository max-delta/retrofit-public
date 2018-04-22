#include "stdafx.h"

#include "AppCommon_GraphicalClient/Common.h"
#include "FramePackEditor.h"

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

int main()
{
	using namespace RF;

	app::Startup();

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing framepack editor..." );
	UniquePtr<FramePackEditor> framePackEditorPtr = EntwinedCreator<FramePackEditor>::Create();
	FramePackEditor* const framePackEditor = framePackEditorPtr;
	framePackEditor->Init();

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Startup complete" );

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
			framePackEditor->Process();
			framePackEditor->Render();

			app::g_Graphics->SubmitToRender();
			app::g_Graphics->WaitForRender();
		}
		app::g_Graphics->EndFrame();
	}
	framePackEditorPtr = nullptr;
	app::Shutdown();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
