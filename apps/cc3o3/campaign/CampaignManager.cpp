#include "stdafx.h"
#include "CampaignManager.h"

#include "cc3o3/appstates/AppStatesFwd.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"


namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

std::string CampaignManager::DetermineOverworldAreaLocKey( rftl::string identifier )
{
	// TODO: Under some conditions, this may change, such as to a key for "???"
	return rftl::string( "$location_area_" ) + identifier;
}

//

bool CampaignManager::TryInteractWithOverworldArea( appstate::AppStateTickContext& context, rftl::string identifier )
{
	// HACK: Just pop into site
	// TODO: Make sure party can actually enter site, and set it up
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Site );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
