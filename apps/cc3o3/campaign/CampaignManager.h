#pragma once
#include "cc3o3/campaign/CampaignFwd.h"

#include "cc3o3/overworld/OverworldFwd.h"
#include "cc3o3/site/SiteFwd.h"

#include "GameAppState/AppStateFwd.h"

#include "rftl/string"


namespace RF::cc::campaign {
///////////////////////////////////////////////////////////////////////////////

// The campaign manager abstracts away highly bespoke questions such as:
//  * Can we walk through this area yet?
//  * Will entering this area take us to a site, a battle, or a cutscene?
//  * What happens after we finish this encounter? Another battle? A cutscene?
//  * Have we won the game yet? Should we show the credits now?
class CampaignManager
{
	//
	// Public methods
public:
	CampaignManager() = default;

	overworld::Overworld LoadDataForOverworld();

	std::string DetermineOverworldAreaLocKey(
		rftl::string identifier );
	bool TryInteractWithOverworldArea(
		appstate::AppStateTickContext& context,
		rftl::string identifier );

	site::Site LoadDataForSite();


	//
	// Private data
private:
	// TODO
};

///////////////////////////////////////////////////////////////////////////////
}