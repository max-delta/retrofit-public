#include "stdafx.h"
#include "FightController.h"

#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/input/HardcodedSetup.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"

#include "rftl/vector"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

FightController::FightController(
	WeakPtr<CombatEngine const> const& combatEngine,
	WeakPtr<company::CompanyManager const> const& companyManager )
{
	mCombatInstance = DefaultCreator<CombatInstance>::Create( combatEngine );
}



WeakPtr<CombatInstance const> FightController::GetCombatInstance() const
{
	return mCombatInstance;
}



PartyID FightController::GetLocalPartyID() const
{
	return mLocalPartyID;
}



void FightController::HardcodedPlaceholderSetup()
{
	CombatInstance& instance = *mCombatInstance;

	// Setup players
	// HACK: One player
	// TODO: Multiplayer, opposing teams
	// TODO: Multiplayer, same team
	rftl::vector<input::PlayerID> const playerIDs = { input::HardcodedGetLocalPlayer() };
	for( input::PlayerID const& playerID : playerIDs )
	{
		TeamID const playerTeam = instance.AddTeam();
		PartyID const playerParty = instance.AddParty( playerTeam );
		if( playerID == input::HardcodedGetLocalPlayer() )
		{
			mLocalPartyID = playerParty;
		}

		// Get the active party characters
		rftl::array<state::MutableObjectRef, 3> const activePartyCharacters =
			mCompanyManager->FindMutableActivePartyObjects( input::HardcodedGetLocalPlayer() );

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
			FighterID const fighter = instance.AddFighter( playerParty );
			instance.SetCombatant( fighter, character );
		}
	}

	// Setup NPCs
	// HACK: Hard-coded
	// TODO: Encounters
	if constexpr( true )
	{
		TeamID const enemyTeam = instance.AddTeam();
		PartyID const enemyParty = instance.AddParty( enemyTeam );

		rftl::array<Fighter, 1> enemies = {};
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

			FighterID const enemyID = instance.AddFighter( enemyParty );
			instance.SetCombatant( enemyID, enemy );
		}
	}

	// Setup initial field influence
	// HACK: Hard-coded
	// TODO: Encounter specified override or multiplayer-minded hash source
	instance.GenerateFieldInfluence( 0 );
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex ) const
{
	// TODO
	return true;
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex ) const
{
	// TODO
	return true;
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const
{
	// TODO
	return false;
}



bool FightController::CanCharacterCastElement( uint8_t attackerIndex ) const
{
	// TODO
	return false;
}



bool FightController::CanCharacterActivateDefense( uint8_t attackerIndex ) const
{
	// TODO
	return false;
}



bool FightController::CanPartyEndTurn() const
{
	// TODO
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}
