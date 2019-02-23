#include "stdafx.h"
#include "cc3o3.h"

#include "cc3o3/appstates/Boot.h"
#include "cc3o3/appstates/InitialLoading.h"
#include "cc3o3/appstates/TitleScreen.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

constexpr bool kDebugUI = true;

static appstate::AppStateManager sAppStateManager;

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	sAppStateManager.AddState( appstate::id::Boot, DefaultCreator<appstate::Boot>::Create() );
	sAppStateManager.AddState( appstate::id::InitialLoading, DefaultCreator<appstate::InitialLoading>::Create() );
	sAppStateManager.AddState( appstate::id::TitleScreen, DefaultCreator<appstate::TitleScreen>::Create() );

	sAppStateManager.Start( appstate::id::Boot );
}



void ProcessFrame()
{
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
}



void Shutdown()
{
	sAppStateManager.Stop();
}

///////////////////////////////////////////////////////////////////////////////
}}
