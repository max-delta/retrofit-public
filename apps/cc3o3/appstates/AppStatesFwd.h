#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::appstate;

namespace id {
enum : AppStateID
{
	Boot = 0,

	InitialLoading,

	DevTestCombatCharts,
	DevTestElementLab,
	DevTestGridCharts,
	DevTestLobby,
	DevTestRollback,

	TitleScreen,
	TitleScreen_MainMenu,
	TitleScreen_CharCreate,
	TitleScreen_Options,

	Gameplay,
	Gameplay_Overworld,
	Gameplay_Site,
	Gameplay_Battle,
	Gameplay_Menus
};
}


class Boot;

class InitialLoading;

class DevTestCombatCharts;
class DevTestElementLab;
class DevTestGridCharts;
class DevTestLobby;
class DevTestRollback;

class TitleScreen;
class TitleScreen_MainMenu;
class TitleScreen_CharCreate;
class TitleScreen_Options;

///////////////////////////////////////////////////////////////////////////////
}
