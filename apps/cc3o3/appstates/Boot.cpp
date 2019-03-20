#include "stdafx.h"
#include "Boot.h"

#include "cc3o3/input/HardcodedSetup.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "PPU/PPUController.h"

#include "PlatformFilesystem/VFS.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

static bool sHasBooted = false;

///////////////////////////////////////////////////////////////////////////////

void Boot::OnEnter( AppStateChangeContext& context )
{
	//
}



void Boot::OnExit( AppStateChangeContext& context )
{
	//
}



void Boot::OnTick( AppStateTickContext& context )
{
	// Only allowed to boot once
	RF_ASSERT( sHasBooted == false );
	sHasBooted = true;

	// Debug in case something goes wrong and we get stuck in boot
	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "Booting..." );

	// VFS setup for game data
	file::VFSPath const mountFile = file::VFS::kRoot.GetChild( "config", "vfs_cc3o3.ini" );
	bool vfsTestDataLoad = app::gVfs->AttemptSubsequentMount( mountFile );
	if( vfsTestDataLoad == false )
	{
		RFLOG_FATAL( mountFile, RFCAT_STARTUP, "Can't load cc3o3 mount file" );
	}

	// HACK: Spin up hard-coded input
	// TODO: Get this from a file
	input::HardcodedSetup();

	// Hop to loading ASAP
	context.mManager.RequestDeferredStateChange( id::InitialLoading );
}

///////////////////////////////////////////////////////////////////////////////
}}}
