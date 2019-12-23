#include "stdafx.h"
#include "Boot.h"

#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/input/HardcodedSetup.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "PPU/PPUController.h"


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

	// HACK: Spin up hard-coded input
	// TODO: Get this from a file
	input::HardcodedSetup();

	// Hop out of boot ASAP
	context.mManager.RequestDeferredStateChange( GetStateAfterBoot() );
}

///////////////////////////////////////////////////////////////////////////////
}}}
