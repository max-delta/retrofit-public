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
#include "Timing/Limiter.h"

#include "core_math/math_bits.h"
#include "core/ptr/default_creator.h"

#include "rftl/thread"


///////////////////////////////////////////////////////////////////////////////

int main()
{
	using namespace RF;

	app::Startup();

	file::VFSPath const testDataMountFile = file::VFS::kRoot.GetChild( "config", "vfs_test.ini" );
	bool vfsTestDataLoad = app::gVfs->AttemptSubsequentMount( file::kMountPriorityLowest, testDataMountFile );
	if( vfsTestDataLoad == false )
	{
		RFLOG_FATAL( testDataMountFile, RFCAT_STARTUP, "Can't load test data mount file" );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing framepack editor..." );
	UniquePtr<FramePackEditor> framePackEditorPtr = DefaultCreator<FramePackEditor>::Create( app::gVfs );
	FramePackEditor* const framePackEditor = framePackEditorPtr;
	framePackEditor->Init();

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Startup complete" );

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
			framePackEditor->Process();
			framePackEditor->Render();

			app::gGraphics->SubmitToRender();
			app::gGraphics->WaitForRender();
		}
		app::gGraphics->EndFrame();
	}
	framePackEditorPtr = nullptr;
	app::Shutdown();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
