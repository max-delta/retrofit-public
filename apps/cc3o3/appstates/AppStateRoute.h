#pragma once
#include "project.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "rftl/string"

namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

// Except under extraordinary circumstances, state should start in boot and go
//  into initial loading
AppStateID GetBootState();
AppStateID GetStateAfterBoot();

AppStateID GetStateAfterInitialLoad();
AppStateID GetFirstTitleScreenState();
AppStateID GetFirstGameplayState();

rftl::string GetFallbackOverworldIdentifier();
rftl::string GetFallbackSiteIdentifier();
rftl::string GetFallbackEncounterIdentifier();

///////////////////////////////////////////////////////////////////////////////
}
