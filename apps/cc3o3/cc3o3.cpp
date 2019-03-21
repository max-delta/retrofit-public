#include "stdafx.h"
#include "cc3o3.h"

#include "cc3o3/appstates/Boot.h"
#include "cc3o3/appstates/InitialLoading.h"
#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/input/HardcodedSetup.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"

#include "PPU/PPUController.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

constexpr bool kDebugGrid = true;
constexpr bool kDebugUI = true;
constexpr bool kDebugUIScaler = true;

static appstate::AppStateManager sAppStateManager;

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	sAppStateManager.AddState( appstate::id::Boot, DefaultCreator<appstate::Boot>::Create() );
	sAppStateManager.AddState( appstate::id::InitialLoading, DefaultCreator<appstate::InitialLoading>::Create() );
	sAppStateManager.AddState( appstate::id::TitleScreen, DefaultCreator<appstate::TitleScreen>::Create() );

	sAppStateManager.Start( appstate::id::Boot );

	app::gGraphics->DebugSetGridEnabled( kDebugGrid );

	if( kDebugUIScaler )
	{
		app::gUiManager->SetRootAABBReduction( 1 );
		app::gUiManager->SetDebugAABBReduction( 2 );
	}
}



void ProcessFrame()
{
	// HACK: Tick hard-coded input
	// TODO: Have an input processing tree that handles dependency-based update
	//  logic for all the controllers
	input::HardcodedTick();

	time::FrameClock::time_point const previous = time::FrameClock::previous();
	time::FrameClock::time_point const now = time::FrameClock::now();
	sAppStateManager.Tick( now, now - previous );

	sAppStateManager.ApplyDeferredStateChange();

	ui::ContainerManager& uiMan = *app::gUiManager;
	uiMan.RecalcRootContainer();
	uiMan.ProcessRecalcs();
	if( kDebugUI )
	{
		uiMan.DebugRender();
	}
	uiMan.Render();
}



void Shutdown()
{
	sAppStateManager.Stop();
}

///////////////////////////////////////////////////////////////////////////////
}}
