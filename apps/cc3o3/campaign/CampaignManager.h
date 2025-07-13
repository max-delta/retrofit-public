#pragma once
#include "cc3o3/campaign/CampaignFwd.h"

#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/overworld/OverworldFwd.h"
#include "cc3o3/save/SaveFwd.h"
#include "cc3o3/site/SiteFwd.h"

#include "GameAppState/AppStateFwd.h"

#include "core/macros.h"
#include "core/ptr/unique_ptr.h"

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
	RF_NO_COPY( CampaignManager );

	//
	// Public methods
public:
	CampaignManager();

	bool PrepareCampaign( save::SaveBlob const& saveBlob );
	bool LoadCampaignProgress( save::SaveBlob const& saveBlob );
	bool SaveCampaignProgress( save::SaveBlob& saveBlob );

	void PrepareFastTrackOverworld();
	void PrepareFastTrackSite();
	void PrepareFastTrackMenus();
	void PrepareFastTrackBattle();
	void PrepareFastTrackCutscene();
	appstate::AppStateID PrepareInitialGameplayState();

	void HardcodedSinglePlayerCharacterLoad();
	void HardcodedSinglePlayerObjectSetup();
	void HardcodedSinglePlayerApplyProgression();

	void EnterMenus( appstate::AppStateTickContext& context );
	void LeaveMenus( appstate::AppStateTickContext& context );

	void TODO_ChangeOverworldLocation( int todo );
	void TravelToOverworld(
		appstate::AppStateTickContext& context );
	overworld::Overworld LoadDataForOverworld();

	rftl::string DetermineOverworldAreaLocKey(
		rftl::string identifier );
	bool TryInteractWithOverworldArea(
		appstate::AppStateTickContext& context,
		rftl::string identifier );

	site::Site LoadDataForSite();
	bool CanLeaveSite();
	void LeaveSite( appstate::AppStateTickContext& context );

	void StartEncounter(
		appstate::AppStateTickContext& context,
		rftl::string identifier );

	void HardcodedCombatSetup( combat::FightController& fight );


	//
	// Private methods
private:
	void VerifyCampaignLoaded();


	//
	// Private data
private:
	rftl::string mCampaignName;
	UniquePtr<Campaign const> mCampaign;

	rftl::string mLastOverworldTransition;
	rftl::string mLastSiteTransition;
	rftl::string mLastEncounterTransition;
};

///////////////////////////////////////////////////////////////////////////////
}
