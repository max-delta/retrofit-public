#include "stdafx.h"
#include "FightController.h"

#include "cc3o3/combat/Attack.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "Logging/Logging.h"

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
	: mCombatEngine( combatEngine )
	, mCompanyManager( companyManager )
{
	mCombatInstance = DefaultCreator<CombatInstance>::Create( combatEngine );
}



WeakPtr<CombatInstance const> FightController::GetCombatInstance() const
{
	return mCombatInstance;
}



void FightController::HardcodedPlaceholderSetup( input::PlayerID singlePlayerHack )
{
	RF_ASSERT( mFrameActive == false );

	CombatInstance& instance = *mCombatInstance;

	// Setup players
	// HACK: One player
	// TODO: Multiplayer, opposing teams
	// TODO: Multiplayer, same team
	rftl::vector<input::PlayerID> const playerIDs = { singlePlayerHack };
	for( input::PlayerID const& playerID : playerIDs )
	{
		TeamID const playerTeam = instance.AddTeam();
		PartyID const playerParty = instance.AddParty( playerTeam );
		if( playerID == singlePlayerHack )
		{
			mLocalPartyID = playerParty;
		}

		// Get the active party characters
		rftl::array<state::MutableObjectRef, 3> const activePartyCharacters =
			mCompanyManager->FindMutableActivePartyObjects( singlePlayerHack );

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

	// Initial save
	instance.CommitCombatData();
}



void FightController::StartCombatFrame()
{
	RF_ASSERT( mFrameActive == false );

	mCombatInstance->ReloadCombatData();
	// TODO: Load attack buffer

	mFrameActive = true;
}



void FightController::EndCombatFrame()
{
	RF_ASSERT( mFrameActive );

	mCombatInstance->CommitCombatData();
	// TODO: Save attack buffer

	mFrameActive = false;
}



void FightController::TickPendingActions()
{
	RF_ASSERT( mFrameActive );

	// TODO: Actions have animations and other delays, which need to be handled
	// NOTE: Probably want a seperate system to handle that aspect, since it
	//  needs to work for other parties as well. Ownership of the main combat
	//  instance will probably need to be moved into that new system.

	CombatInstance& combatInstance = *mCombatInstance;

	if( mAttackBuffer.empty() == false )
	{
		// Pop front
		BufferedAttack const attack = mAttackBuffer.front();
		mAttackBuffer.erase( mAttackBuffer.begin() );

		if( combatInstance.CanPerformAttack( attack.mAttackerID, attack.mDefenderID, attack.mAttackStrength ) )
		{
			// Attack
			AttackResult const result = combatInstance.ExecuteAttack( attack.mAttackerID, attack.mDefenderID, attack.mAttackStrength );

			// TODO: Record to some 'recent attacks' buffer for displaying
			// NOTE: Will likely be the responsibility of whatever system is
			//  monitoring and executing the delays and animations, since we
			//  need to display this for other parties as well
			( (void)result );
		}
		else
		{
			RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Buffered attack invalid, cancelling all attacks" );
			RF_DBGFAIL_MSG( "Bad attack. How did this happen? Fix, or flag as benign and document behavior." );
			mAttackBuffer.clear();
		}
	}
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
	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	return mCombatInstance->CanPerformAttack( attackerID );
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex ) const
{
	if( CanCharacterPerformAttack( attackerIndex ) == false )
	{
		return false;
	}

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );
	return mCombatInstance->CanPerformAttack( attackerID, defenderID );
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const
{
	if( CanCharacterPerformAttack( attackerIndex, defenderIndex ) == false )
	{
		return false;
	}

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );

	if( mAttackBuffer.empty() )
	{
		return mCombatInstance->CanPerformAttack( attackerID, defenderID, attackStrength );
	}
	else
	{
		// Attacks are queued, simulate them along with this additional attack
		CombatInstance simulation = *mCombatInstance;
		bool const simSuccess = SimulateAttackBuffer( simulation, attackerID, defenderID, attackStrength );
		return simSuccess;
	}
}



void FightController::PredictAttack( AttackProfile& profile, AttackResult& result, uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const
{
	RF_ASSERT( CanCharacterPerformAttack( attackerIndex, defenderIndex, attackStrength ) );

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );

	CombatInstance simulation = *mCombatInstance;
	if( mAttackBuffer.empty() == false )
	{
		bool const simSuccess = SimulateAttackBuffer( simulation, attackerID, defenderID );

		// The CanCharacterPerformAttack(...) check should already cover this
		RF_ASSERT( simSuccess );
	}

	// The CanCharacterPerformAttack(...) check should already cover this
	RF_ASSERT( simulation.CanPerformAttack( attackerID, defenderID, attackStrength ) );

	profile = simulation.PrepareAttack( attackerID, defenderID, attackStrength );
	result = simulation.ExecuteAttack( attackerID, defenderID, attackStrength );
}



void FightController::BufferAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength )
{
	RF_ASSERT( mFrameActive );

	// TODO: This buffering needs to be re-designed to be stored into a
	//  rollback domain, probably something like:
	//  * buffered:Attacker
	//  * buffered:Defender
	//  * buffered:Attack:0
	//  * buffered:Attack:1
	//  * buffered:Attack:2 (Or whatever limit is reasonable)

	RF_ASSERT( CanCharacterPerformAttack( attackerIndex, defenderIndex, attackStrength ) );
	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );
	if( mAttackBuffer.empty() == false )
	{
		RF_ASSERT( mAttackBuffer.front().mAttackerID == attackerID );
		RF_ASSERT( mAttackBuffer.front().mDefenderID == defenderID );
	}
	mAttackBuffer.emplace_back( { attackerID, defenderID, attackStrength } );
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

bool FightController::SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID ) const
{
	RF_ASSERT( mAttackBuffer.empty() == false );

	for( BufferedAttack const& attack : mAttackBuffer )
	{
		RF_ASSERT( attack.mAttackerID == attackerID );
		RF_ASSERT( attack.mDefenderID == defenderID );
		if( simulation.CanPerformAttack( attackerID, defenderID, attack.mAttackStrength ) == false )
		{
			return false;
		}
		simulation.ExecuteAttack( attackerID, defenderID, attack.mAttackStrength );
	}
	return true;
}



bool FightController::SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID, uint8_t additionalAttackStrength ) const
{
	RF_ASSERT( mAttackBuffer.empty() == false );

	// Buffered attacks
	bool const bufferSuccessful = SimulateAttackBuffer( simulation, attackerID, defenderID );
	if( bufferSuccessful == false )
	{
		return false;
	}

	// Additional attack
	if( simulation.CanPerformAttack( attackerID, defenderID, additionalAttackStrength ) == false )
	{
		return false;
	}
	simulation.ExecuteAttack( attackerID, defenderID, additionalAttackStrength );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
