#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::appstate;

namespace id {
enum : AppStateID
{
	Boot = 0,

	InitialLoading,

	DevTestCombatCharts,

	TitleScreen,
	TitleScreen_MainMenu,
	TitleScreen_CharCreate,
	TitleScreen_Options
};
}


class Boot;

class InitialLoading;

class TitleScreen;
class TitleScreen_MainMenu;

///////////////////////////////////////////////////////////////////////////////
}}}
