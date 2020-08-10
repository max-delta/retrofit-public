#include "stdafx.h"
#include "FightController.h"

#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/input/HardcodedSetup.h"

#include "core_component/TypedObjectRef.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "core/ptr/default_creator.h"

#include "rftl/algorithm"
#include "rftl/vector"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////
namespace details {

uint8_t SanitizeFighterIndex( CombatInstance::FighterIDs const& ids, uint8_t index, int8_t applyOffset )
{
	RF_ASSERT( ids.empty() == false );

	// Convert to indices
	using Indices = rftl::static_vector<uint8_t, CombatInstance::FighterIDs::fixed_capacity>;
	Indices indices;
	for( FighterID const& id : ids )
	{
		indices.emplace_back( id.GetFighterIndex() );
	}
	RF_ASSERT( rftl::is_sorted( indices.begin(), indices.end() ) );

	// Find current position in list
	Indices::const_iterator iter = rftl::lower_bound( indices.begin(), indices.end(), index );
	if( iter == indices.end() )
	{
		// Loop around
		iter = indices.begin();
	}
	uint8_t const currentOffset = math::integer_cast<uint8_t>( rftl::distance( indices.cbegin(), iter ) );

	// Apply offset and return lookup
	uint8_t const targetOffset = math::WrapPositiveOffset<uint8_t>(
		currentOffset,
		math::integer_cast<uint8_t>( indices.size() ),
		applyOffset );
	return indices.at( targetOffset );
}

}
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
			FighterID const enemyID = instance.AddFighter( enemyParty );
			instance.SetCombatant( enemyID, enemy );
		}
	}

	// Setup initial field influence
	// HACK: Hard-coded
	// TODO: Encounter specified override or multiplayer-minded hash source
	instance.GenerateFieldInfluence( 0 );
}



uint8_t FightController::SanitizeCharacterIndex( uint8_t attackerIndex, int8_t applyOffset ) const
{
	CombatInstance::FighterIDs const characters = mCombatInstance->GetFighterIDs( mLocalPartyID );
	return details::SanitizeFighterIndex( characters, attackerIndex, applyOffset );
}



uint8_t FightController::SanitizeAttackTargetIndex( uint8_t attackerIndex, uint8_t defenderIndex, int8_t applyOffset ) const
{
	FighterID const attacker = GetCharacterByIndex( attackerIndex );
	CombatInstance::FighterIDs const attackTargets = mCombatInstance->GetValidAttackTargets( attacker );
	return details::SanitizeFighterIndex( attackTargets, defenderIndex, applyOffset );
}



PartyID FightController::GetLocalPartyID() const
{
	return mLocalPartyID;
}



FighterID FightController::GetCharacterByIndex( uint8_t attackerIndex ) const
{
	return mLocalPartyID.GetFighterAt( attackerIndex );
}



FighterID FightController::GetAttackTargetByIndex( uint8_t attackerIndex, uint8_t defenderIndex ) const
{
	FighterID const attacker = GetCharacterByIndex( attackerIndex );
	CombatInstance::FighterIDs const attackTargets = mCombatInstance->GetValidAttackTargets( attacker );
	RF_ASSERT( defenderIndex < attackTargets.size() );
	return attackTargets.at( defenderIndex );
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
