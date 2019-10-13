#include "stdafx.h"
#include "AppStateRoute.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////
namespace fasttrack {

enum class Mode : uint8_t
{
	// If invalid, FastTrack is not in use
	Invalid = 0,

	// Various developer tests
	DevTestCombatCharts,
	DevTestRollback,

	// Directly to options menu on the title screen
	TitleScreenOptions,

	// Directly to character creation on the title screen
	TitleScreenCharCreate
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
	else if( fasttrack::sMode == fasttrack::Mode::DevTestRollback )
	{
		return id::DevTestRollback;
	}

	return id::TitleScreen;
}



AppStateID GetFirstTitleScreenState()
{
	if( fasttrack::sMode == fasttrack::Mode::TitleScreenOptions )
	{
		return id::TitleScreen_Options;
	}
	else if( fasttrack::sMode == fasttrack::Mode::TitleScreenCharCreate )
	{
		return id::TitleScreen_CharCreate;
	}

	return id::TitleScreen_MainMenu;
}

///////////////////////////////////////////////////////////////////////////////
}}}
