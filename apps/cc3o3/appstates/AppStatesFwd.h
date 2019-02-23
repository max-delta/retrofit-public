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
	TitleScreen
};
}

class Boot;
class InitialLoading;
class TitleScreen;

///////////////////////////////////////////////////////////////////////////////
}}}
