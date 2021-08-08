#include "stdafx.h"
#include "CampaignManager.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/AppStatesFwd.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/combat/FightController.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/overworld/Overworld.h"
#include "cc3o3/site/Site.h"

#include "core/ptr/default_creator.h"

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



void CampaignManager::HardcodedCombatSetup( combat::FightController& fight )
{
	using namespace combat;

	UniquePtr<CombatInstance> instance = DefaultCreator<CombatInstance>::Create( gCombatEngine );
	CombatInstance& setup = *instance;

	input::PlayerID const singlePlayerHack = appstate::InputHelpers::GetSinglePlayer();

	PartyID localPartyID = {};

	// Setup players
	// HACK: One player
	// TODO: Multiplayer, opposing teams
	// TODO: Multiplayer, same team
	rftl::vector<input::PlayerID> const playerIDs = { singlePlayerHack };
	for( input::PlayerID const& playerID : playerIDs )
	{
		TeamID const playerTeam = setup.AddTeam();
		PartyID const playerParty = setup.AddParty( playerTeam );
		if( playerID == singlePlayerHack )
		{
			localPartyID = playerParty;
		}

		// Get the active party characters
		rftl::array<state::MutableObjectRef, 3> const activePartyCharacters =
			gCompanyManager->FindMutableActivePartyObjects( singlePlayerHack );

		// For each active team member...
		for( size_t i_teamIndex = 0; i_teamIndex < company::kActiveTeamSize; i_teamIndex++ )
		{
			state::MutableObjectRef const& character = activePartyCharacters.at( i_teamIndex );
			if( character.IsSet() == false )
			{
				// Not active
				continue;
			}

			// Add to party
			FighterID const fighter = setup.AddFighter( playerParty );
			setup.SetCombatant( fighter, character );
		}
	}

	// Setup NPCs
	// HACK: Hard-coded
	// TODO: Encounters
	if constexpr( true )
	{
		TeamID const enemyTeam = setup.AddTeam();
		PartyID const enemyParty = setup.AddParty( enemyTeam );

		rftl::array<Fighter, 2> enemies = {};
		{
			Fighter& enemy = enemies.at( 0 );
			enemy.mInnate = element::MakeInnateIdentifier( element::InnateString{ 'r', 'e', 'd' } );
			enemy.mMaxHealth = 999;
			enemy.mCurHealth = enemy.mMaxHealth;
			enemy.mMaxStamina = 7;
			enemy.mCurStamina = enemy.mMaxStamina;
			enemy.mPhysAtk = 2;
			enemy.mPhysDef = 2;
			enemy.mElemAtk = 2;
			enemy.mElemDef = 2;
			enemy.mBalance = 2;
			enemy.mTechniq = 2;
		}
		{
			Fighter& enemy = enemies.at( 1 );
			enemy.mInnate = element::MakeInnateIdentifier( element::InnateString{ 'r', 'e', 'd' } );
			enemy.mMaxHealth = 999;
			enemy.mCurHealth = enemy.mMaxHealth;
			enemy.mMaxStamina = 7;
			enemy.mCurStamina = enemy.mMaxStamina;
			enemy.mPhysAtk = 2;
			enemy.mPhysDef = 2;
			enemy.mElemAtk = 2;
			enemy.mElemDef = 2;
			enemy.mBalance = 2;
			enemy.mTechniq = 2;
		}
		for( Fighter const& enemy : enemies )
		{
			FighterID const enemyID = setup.AddFighter( enemyParty );
			setup.SetCombatant( enemyID, enemy );
		}
	}

	// Setup initial field influence
	// HACK: Hard-coded
	// TODO: Encounter specified override or multiplayer-minded hash source
	setup.GenerateFieldInfluence( 0 );

	fight.SetupFromCombatInstance( rftl::move( instance ), localPartyID );
}

///////////////////////////////////////////////////////////////////////////////
}
