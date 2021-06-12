#include "stdafx.h"
#include "CampaignManager.h"

#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/AppStatesFwd.h"
#include "cc3o3/overworld/Overworld.h"
#include "cc3o3/site/Site.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"


namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

overworld::Overworld CampaignManager::LoadDataForOverworld()
{
	// Load map
	// HACK: Hard-coded
	// TODO: Figure out from current pawn position
	static constexpr char const kHackMap[] = "island1";
	file::VFSPath const mapDescPath = paths::TablesRoot().GetChild( "world", "overworlds", rftl::string( kHackMap ) + ".oo" );
	return overworld::Overworld::LoadFromDesc( mapDescPath );
}



std::string CampaignManager::DetermineOverworldAreaLocKey( rftl::string identifier )
{
	// TODO: Under some conditions, this may change, such as to a key for "???"
	return rftl::string( "$location_area_" ) + identifier;
}



bool CampaignManager::TryInteractWithOverworldArea( appstate::AppStateTickContext& context, rftl::string identifier )
{
	// HACK: Just pop into site
	// TODO: Make sure party can actually enter site, and set it up
	context.mManager.RequestDeferredStateChange( appstate::id::Gameplay_Site );

	return true;
}



site::Site CampaignManager::LoadDataForSite()
{
	// Load site
	// HACK: Hard-coded
	// TODO: Figure out from current pawn position
	static constexpr char const kHackSite[] = "temp";
	file::VFSPath const siteDescPath = paths::TablesRoot().GetChild( "world", "sites", rftl::string( kHackSite ) + ".oo" );
	return site::Site::LoadFromDesc( siteDescPath );
}

///////////////////////////////////////////////////////////////////////////////
}
