#pragma once
#include "project.h"

#include "GameAppState/AppStateFwd.h"

namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::appstate;

constexpr AppStateID kBoot = 0;
class Boot;

constexpr AppStateID kInitialLoading = 1;
class InitialLoading;

constexpr AppStateID kTitleScreen = 2;
class TitleScreen;

///////////////////////////////////////////////////////////////////////////////
}}}
