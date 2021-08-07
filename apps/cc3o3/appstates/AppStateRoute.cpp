#include "stdafx.h"
#include "AppStateRoute.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////
namespace fasttrack {

enum class Mode : uint8_t
{
	// If invalid, FastTrack is not in use
	Invalid = 0,

	// Various developer tests
	DevTestCombatCharts,
	DevTestGridCharts,
	DevTestLobby,
	DevTestRollback,

	// Directly to options menu on the title screen
	TitleScreenOptions,

	// Directly to character creation on the title screen
	TitleScreenCharCreate,

	// Directly to gameplay
	GameplayOverworld,
	GameplayMenus,
	GameplayBattle
};

static Mode sMode = Mode::Invalid;

}
///////////////////////////////////////////////////////////////////////////////

AppStateID GetBootState()
{
	return id::Boot;
}



AppStateID GetStateAfterBoot()
{
	return id::InitialLoading;
}



AppStateID GetStateAfterInitialLoad()
{
	if( fasttrack::sMode == fasttrack::Mode::DevTestCombatCharts )
	{
		return id::DevTestCombatCharts;
	}
	if( fasttrack::sMode == fasttrack::Mode::DevTestGridCharts )
	{
		return id::DevTestGridCharts;
	}
	if( fasttrack::sMode == fasttrack::Mode::DevTestLobby )
	{
		return id::DevTestLobby;
	}
	if( fasttrack::sMode == fasttrack::Mode::DevTestRollback )
	{
		return id::DevTestRollback;
	}
	if( fasttrack::sMode == fasttrack::Mode::GameplayOverworld )
	{
		return id::Gameplay;
	}
	if( fasttrack::sMode == fasttrack::Mode::GameplayMenus )
	{
		return id::Gameplay;
	}
	if( fasttrack::sMode == fasttrack::Mode::GameplayBattle )
	{
		return id::Gameplay;
	}


	return id::TitleScreen;
}



AppStateID GetFirstTitleScreenState()
{
	if( fasttrack::sMode == fasttrack::Mode::TitleScreenOptions )
	{
		return id::TitleScreen_Options;
	}
	if( fasttrack::sMode == fasttrack::Mode::TitleScreenCharCreate )
	{
		return id::TitleScreen_CharCreate;
	}

	return id::TitleScreen_MainMenu;
}



AppStateID GetFirstGameplayState()
{
	if( fasttrack::sMode == fasttrack::Mode::GameplayOverworld )
	{
		return id::Gameplay_Overworld;
	}
	if( fasttrack::sMode == fasttrack::Mode::GameplayMenus )
	{
		return id::Gameplay_Menus;
	}
	if( fasttrack::sMode == fasttrack::Mode::GameplayBattle )
	{
		return id::Gameplay_Battle;
	}

	// TODO: This should be based on the save file
	return id::Gameplay_Overworld;
}

///////////////////////////////////////////////////////////////////////////////
}
