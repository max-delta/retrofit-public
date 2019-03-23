#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

// Except under extraordinary circumstances, state should start in boot and go
//  into initial loading
AppStateID GetBootState();
AppStateID GetStateAfterBoot();

AppStateID GetStateAfterInitialLoad();
AppStateID GetFirstTitleScreenState();

///////////////////////////////////////////////////////////////////////////////
}}}
