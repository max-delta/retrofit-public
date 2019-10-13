#include "stdafx.h"
#include "cc3o3.h"

#include "cc3o3/appstates/Boot.h"
#include "cc3o3/appstates/InitialLoading.h"
#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/appstates/DevTestCombatCharts.h"
#include "cc3o3/appstates/DevTestRollback.h"
#include "cc3o3/appstates/AppStateRoute.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"

#include "PPU/PPUController.h"

#include "Timing/FrameClock.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

constexpr bool kAllowDebug = true;

constexpr bool kDebugGrid = kAllowDebug && true;
constexpr bool kDebugUI = kAllowDebug && true;
constexpr bool kDebugUIScaler = kAllowDebug && false;
constexpr bool kDebugHideZoomFactor = kAllowDebug && false;
constexpr bool kDebugSuppressRender = kAllowDebug && false;

constexpr bool kAllowDevTests = true;

static appstate::AppStateManager sAppStateManager;

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	SystemStartup();

	sAppStateManager.AddState( appstate::id::Boot, DefaultCreator<appstate::Boot>::Create() );
	sAppStateManager.AddState( appstate::id::InitialLoading, DefaultCreator<appstate::InitialLoading>::Create() );
	sAppStateManager.AddState( appstate::id::TitleScreen, DefaultCreator<appstate::TitleScreen>::Create() );
	if( kAllowDevTests )
	{
		sAppStateManager.AddState( appstate::id::DevTestCombatCharts, DefaultCreator<appstate::DevTestCombatCharts>::Create() );
		sAppStateManager.AddState( appstate::id::DevTestRollback, DefaultCreator<appstate::DevTestRollback>::Create() );
	}

	sAppStateManager.Start( appstate::GetBootState() );

	app::gGraphics->DebugSetGridEnabled( kDebugGrid );
	app::gGraphics->HideZoomFactor( kDebugHideZoomFactor );

	// Leave atleast a pixel around all edges of the screen
	app::gUiManager->SetRootAABBReduction( 1 );

	if( kDebugUIScaler )
	{
		app::gUiManager->SetDebugAABBReduction( 2 );
	}
}



void ProcessFrame()
{
	// HACK: Tick hard-coded input
	// TODO: Have an input processing tree that handles dependency-based update
	//  logic for all the controllers
	input::HardcodedTick();

	time::CommonClock::time_point const previous = time::FrameClock::previous();
	time::CommonClock::time_point const now = time::FrameClock::now();
	sAppStateManager.Tick( now, now - previous );
	sAppStateManager.ApplyDeferredStateChange();

	ui::ContainerManager& uiMan = *app::gUiManager;
	uiMan.RecalcRootContainer();
	uiMan.ProcessRecalcs();
	if( kDebugUI )
	{
		uiMan.DebugRender( kDebugSuppressRender );
	}
	if( kDebugSuppressRender == false )
	{
		uiMan.Render();
	}
}



void Shutdown()
{
	sAppStateManager.Stop();

	SystemShutdown();
}

///////////////////////////////////////////////////////////////////////////////
}}
