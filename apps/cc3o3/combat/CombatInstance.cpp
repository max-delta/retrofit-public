#include "stdafx.h"
#include "CombatInstance.h"

#include "cc3o3/combat/Attack.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Character.h"
#include "cc3o3/state/StateLogging.h"

#include "core_component/TypedComponentRef.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"
#include "core_math/Rand.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

CombatInstance::CombatInstance( WeakPtr<CombatEngine const> const& combatEngine )
	: mCombatEngine( combatEngine )
{
	//
}



TeamID CombatInstance::AddTeam()
{
	RF_ASSERT( mTeams.size() < kMaxTeamsPerFight );
	mTeams.emplace_back();
	return TeamID::MakeTeam( math::integer_cast<TeamIndex>( mTeams.size() - 1 ) );
}



PartyID CombatInstance::AddParty( TeamID teamID )
{
	Parties& parties = GetMutableTeamRef( teamID ).mParties;
	RF_ASSERT( parties.size() < kMaxPartiesPerTeam );
	parties.emplace_back();
	return teamID.GetPartyAt( math::integer_cast<PartyIndex>( parties.size() - 1 ) );
}



FighterID CombatInstance::AddFighter( PartyID partyID )
{
	Fighters& fighters = GetMutablePartyRef( partyID ).mFighters;
	RF_ASSERT( fighters.size() < kMaxFightersPerParty );
	fighters.emplace_back();
	return partyID.GetFighterAt( math::integer_cast<FighterIndex>( fighters.size() - 1 ) );
}



void CombatInstance::RemoveFighter( FighterID fighterID )
{
	Fighters& fighters = GetMutablePartyRef( fighterID.GetParty() ).mFighters;
	RF_ASSERT( fighterID.GetFighterIndex() < fighters.size() );
	fighters.erase( fighters.begin() + fighterID.GetFighterIndex() );
}



CombatInstance::TeamIDs CombatInstance::GetTeamIDs() const
{
	TeamIDs retVal = {};
	for( TeamIndex i_team = 0; i_team < mTeams.size(); i_team++ )
	{
		retVal.emplace_back( TeamID::MakeTeam( i_team ) );
	}
	return retVal;
}



CombatInstance::PartyIDs CombatInstance::GetPartyIDs() const
{
	PartyIDs retVal = {};
	for( TeamIndex i_team = 0; i_team < mTeams.size(); i_team++ )
	{
		TeamEntry const& team = mTeams.at( i_team );
		for( PartyIndex i_party = 0; i_party < team.mParties.size(); i_party++ )
		{
			retVal.emplace_back( PartyID::MakeParty( i_team, i_party ) );
		}
	}
	return retVal;
}



CombatInstance::PartyIDs CombatInstance::GetPartyIDs( TeamID teamID ) const
{
	PartyIDs retVal = {};
	TeamEntry const& team = GetTeamRef( teamID );
	for( PartyIndex i_party = 0; i_party < team.mParties.size(); i_party++ )
	{
		retVal.emplace_back( teamID.GetPartyAt( i_party ) );
	}
	return retVal;
}



CombatInstance::FighterIDs CombatInstance::GetFighterIDs() const
{
	FighterIDs retVal = {};
	for( TeamIndex i_team = 0; i_team < mTeams.size(); i_team++ )
	{
		TeamEntry const& team = mTeams.at( i_team );
		for( PartyIndex i_party = 0; i_party < team.mParties.size(); i_party++ )
		{
			PartyEntry const& party = team.mParties.at( i_party );
			for( FighterIndex i_fighter = 0; i_fighter < party.mFighters.size(); i_fighter++ )
			{
				retVal.emplace_back( FighterID::MakeFighter( i_team, i_party, i_fighter ) );
			}
		}
	}
	return retVal;
}



CombatInstance::FighterIDs CombatInstance::GetFighterIDs( TeamID teamID ) const
{
	FighterIDs retVal = {};
	TeamEntry const& team = GetTeamRef( teamID );
	for( PartyIndex i_party = 0; i_party < team.mParties.size(); i_party++ )
	{
		PartyEntry const& party = team.mParties.at( i_party );
		for( FighterIndex i_fighter = 0; i_fighter < party.mFighters.size(); i_fighter++ )
		{
			retVal.emplace_back( teamID.GetPartyAt( i_party ).GetFighterAt( i_fighter ) );
		}
	}
	return retVal;
}



CombatInstance::FighterIDs CombatInstance::GetFighterIDs( PartyID partyID ) const
{
	FighterIDs retVal = {};
	PartyEntry const& party = GetPartyRef( partyID );
	for( FighterIndex i_fighter = 0; i_fighter < party.mFighters.size(); i_fighter++ )
	{
		retVal.emplace_back( partyID.GetFighterAt( i_fighter ) );
	}
	return retVal;
}



CombatInstance::TeamIDs CombatInstance::GetOpposingTeams( TeamID teamID ) const
{
	TeamIDs retVal = {};
	for( TeamIndex i_team = 0; i_team < mTeams.size(); i_team++ )
	{
		if( i_team != teamID.GetTeamIndex() )
		{
			retVal.emplace_back( TeamID::MakeTeam( i_team ) );
		}
	}
	return retVal;
}



Team CombatInstance::GetTeam( TeamID teamID ) const
{
	return GetTeamRef( teamID ).mTeam;
}



Party CombatInstance::GetParty( PartyID partyID ) const
{
	return GetPartyRef( partyID ).mParty;
}



Fighter CombatInstance::GetFighter( FighterID fighterID ) const
{
	return GetFighterRef( fighterID ).mFighter;
}



void CombatInstance::SetCombatant( FighterID fighterID, Fighter const& combatant )
{
	FighterEntry& fighter = GetMutableFighterRef( fighterID );
	fighter.mFighter = combatant;
	RF_ASSERT_MSG( fighter.mPersist.IsSet() == false, "Cutting combatant persistence, dubious" );
	fighter.mPersist = {};
}



void CombatInstance::SetCombatant( FighterID fighterID, state::MutableObjectRef const& character )
{
	FighterEntry& fighter = GetMutableFighterRef( fighterID );
	fighter.mFighter = {};
	fighter.mPersist = character;
	LoadFighterFromCharacter( fighter.mFighter, fighter.mPersist );
	InitializeFighter( fighter.mFighter );
}



void CombatInstance::CommitCombatData() const
{
	// TODO: Field data
	// TODO: Team data
	// TODO: Party data

	// Character data
	FighterIDs const fighterIDs = GetFighterIDs();
	for( FighterID const& fighterID : fighterIDs )
	{
		FighterEntry const& fighter = GetFighterRef( fighterID );
		if( fighter.mPersist.IsSet() )
		{
			SaveFighterToCharacter( fighter.mPersist, fighter.mFighter );
		}
	}
}



void CombatInstance::ReloadCombatData()
{
	// TODO: Field data
	// TODO: Team data
	// TODO: Party data

	// Character data
	FighterIDs const fighterIDs = GetFighterIDs();
	for( FighterID const& fighterID : fighterIDs )
	{
		FighterEntry& fighter = GetMutableFighterRef( fighterID );
		if( fighter.mPersist.IsSet() )
		{
			LoadFighterFromCharacter( fighter.mFighter, fighter.mPersist );
		}
	}
}



rftl::array<element::InnateIdentifier, kFieldSize> CombatInstance::GetFieldInfluence() const
{
	return mField.mInfluence;
}



void CombatInstance::AddFieldInfluence( element::InnateIdentifier influence )
{
	// Shift everything back 1, push_back()
	for( size_t i = 0; i < kFieldSize - 1; i++ )
	{
		mField.mInfluence.at( i ) = mField.mInfluence.at( i + 1 );
	}
	mField.mInfluence.back() = influence;
}



void CombatInstance::AddFieldInfluence( element::InnateIdentifier influence, size_t strength )
{
	for( size_t i = 0; i < strength; i++ )
	{
		AddFieldInfluence( influence );
	}
}



void CombatInstance::GenerateFieldInfluence( uint64_t seedHash )
{
	uint32_t genVal = math::GetSeedFromHash( seedHash );
	for( size_t i = 0; i < kFieldSize; i++ )
	{
		element::InnateIdentifier const influence = mCombatEngine->GenerateRandomInnate( genVal );
		AddFieldInfluence( influence );
	}
}



SimVal CombatInstance::GetCounterGuage( PartyID party ) const
{
	return GetPartyRef( party ).mParty.mCounterGuage;
}



void CombatInstance::IncreaseCounterGuage( PartyID party, SimVal value )
{
	SimVal& counterGuage = GetMutablePartyRef( party ).mParty.mCounterGuage;
	SimVal const maxIncrease = math::integer_cast<SimVal>( kCounterGaugeMax - counterGuage );
	counterGuage = math::Min( maxIncrease, value );
}



void CombatInstance::IncreaseHealth( FighterID fighterID, SimVal value )
{
	Fighter& fighter = GetMutableFighterRef( fighterID ).mFighter;
	RF_ASSERT( fighter.mMaxHealth <= kMaxHealth );
	RF_ASSERT( fighter.mCurHealth >= kMinHealth );
	RF_ASSERT( fighter.mCurHealth <= fighter.mMaxHealth );
	fighter.mCurHealth = math::Min( math::integer_cast<LargeSimVal>( fighter.mCurHealth + value ), fighter.mMaxHealth );
	RF_ASSERT( fighter.mCurHealth >= kMinHealth );
	RF_ASSERT( fighter.mCurHealth <= fighter.mMaxHealth );
}



void CombatInstance::DecreaseHealth( FighterID fighterID, SimVal value )
{
	Fighter& fighter = GetMutableFighterRef( fighterID ).mFighter;
	RF_ASSERT( fighter.mMaxHealth <= kMaxHealth );
	RF_ASSERT( fighter.mCurHealth >= kMinHealth );
	RF_ASSERT( fighter.mCurHealth <= fighter.mMaxHealth );
	static_assert( kMinHealth == 0 );
	LargeSimVal const maxLoss = fighter.mCurHealth;
	fighter.mCurHealth -= math::Min( math::integer_cast<LargeSimVal>( value ), maxLoss );
	RF_ASSERT( fighter.mCurHealth >= kMinHealth );
	RF_ASSERT( fighter.mCurHealth <= fighter.mMaxHealth );
}



void CombatInstance::IncreaseStamina( FighterID fighterID, SimVal value )
{
	Fighter& fighter = GetMutableFighterRef( fighterID ).mFighter;
	RF_ASSERT( fighter.mMaxStamina <= kMaxStamina );
	RF_ASSERT( fighter.mCurStamina >= kMinStamina );
	RF_ASSERT( fighter.mCurStamina <= fighter.mMaxStamina );
	fighter.mCurStamina = math::Min( math::integer_cast<SimDelta>( fighter.mCurStamina + value ), fighter.mMaxStamina );
	RF_ASSERT( fighter.mCurStamina >= kMinStamina );
	RF_ASSERT( fighter.mCurStamina <= fighter.mMaxStamina );
}



void CombatInstance::DecreaseStamina( FighterID fighterID, SimVal value )
{
	Fighter& fighter = GetMutableFighterRef( fighterID ).mFighter;
	RF_ASSERT( fighter.mCurStamina >= kMinStamina );
	RF_ASSERT( fighter.mCurStamina <= kMaxStamina );
	fighter.mCurStamina -= value;
	RF_ASSERT( fighter.mCurStamina >= kMinStamina );
	RF_ASSERT( fighter.mCurStamina <= kMaxStamina );
}



CombatInstance::FighterIDs CombatInstance::GetValidAttackTargets( FighterID attackerID ) const
{
	FighterIDs retVal;
	TeamIDs const opposingTeams = GetOpposingTeams( attackerID.GetTeam() );
	for( TeamID const& opposingTeam : opposingTeams )
	{
		FighterIDs const fighters = GetFighterIDs( opposingTeam );
		for( FighterID const& fighter : fighters )
		{
			retVal.emplace_back( fighter );
		}
	}
	return retVal;
}



bool CombatInstance::CanPerformAttack( FighterID attackerID ) const
{
	Fighter const attacker = GetFighter( attackerID );

	if( attacker.mCurHealth <= 0 )
	{
		// Attacker is dead
		return false;
	}

	if( attacker.mCurStamina <= 0 )
	{
		// No stamina
		return false;
	}

	return true;
}



bool CombatInstance::CanPerformAttack( FighterID attackerID, FighterID defenderID ) const
{
	if( CanPerformAttack( attackerID ) == false )
	{
		return false;
	}

	Fighter const defender = GetFighter( defenderID );

	if( defender.mCurHealth <= 0 )
	{
		// Defender is dead
		return false;
	}

	return true;
}



bool CombatInstance::CanPerformAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const
{
	if( CanPerformAttack( attackerID, defenderID ) == false )
	{
		return false;
	}

	Fighter const attacker = GetFighter( attackerID );
	uint8_t const staminaCost = mCombatEngine->LoCalcAttackStaminaCost( attackStrength );

	if( staminaCost > attacker.mCurStamina )
	{
		// Not enough stamina
		return false;
	}

	return true;
}



AttackProfile CombatInstance::PrepareAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const
{
	Fighter const& attacker = GetFighterRef( attackerID ).mFighter;
	Fighter const& defender = GetFighterRef( defenderID ).mFighter;

	AttackProfile retVal = {};

	retVal.mAttackStrength = attackStrength;

	retVal.mNewCombo = true;
	if( attacker.mComboTarget == defenderID )
	{
		retVal.mNewCombo = false;
	}
	retVal.mComboMeter = attacker.mComboMeter;
	retVal.mAttackerTechnique = attacker.mTechniq;
	retVal.mDefenderBalance = defender.mBalance;

	retVal.mAttackerPhysicalAttack = attacker.mPhysAtk;
	retVal.mDefenderPhysicalDefense = defender.mPhysDef;
	retVal.mAttackerInnate = attacker.mInnate;
	retVal.mDefenderInnate = defender.mInnate;
	retVal.mInfluence = GetFieldInfluence();

	return retVal;
}



AttackResult CombatInstance::ExecuteAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength )
{
	CombatEngine const& engine = *mCombatEngine;

	AttackProfile const profile = PrepareAttack( attackerID, defenderID, attackStrength );
	AttackResult const result = engine.HiCalcAttack( profile );

	Fighter& attacker = GetMutableFighterRef( attackerID ).mFighter;

	attacker.mComboTarget = defenderID;
	attacker.mComboMeter = result.mNewComboMeter;
	DecreaseStamina( attackerID, engine.LoCalcAttackStaminaCost( profile.mAttackStrength ) );
	DecreaseHealth( defenderID, result.mDamage );
	IncreaseCounterGuage( defenderID.GetParty(), result.mCoungerGuageIncrease );
	PassTime( attackerID );

	return result;
}



void CombatInstance::PassTime( FighterID initiatorID )
{
	CombatEngine const& engine = *mCombatEngine;

	// Increase stamina pasively
	SimVal const allyStaminaGain = engine.LoCalcIdleStaminaGainAlliedTurn();
	SimVal const opposeStaminaGain = engine.LoCalcIdleStaminaGainOpposingTurn();
	TeamIDs const opposingTeams = GetOpposingTeams( initiatorID.GetTeam() );
	FighterIDs const allFighterIDs = GetFighterIDs();
	for( FighterID const& fighterID : allFighterIDs )
	{
		if( fighterID == initiatorID )
		{
			// Initiator doesn't gain passive stamina
			continue;
		}

		bool isOpposingFighter = false;
		for( TeamID const& opposingTeam : opposingTeams )
		{
			if( fighterID.GetTeam() == opposingTeam )
			{
				isOpposingFighter = true;
				break;
			}
		}

		if( isOpposingFighter )
		{
			IncreaseStamina( fighterID, allyStaminaGain );
		}
		else
		{
			IncreaseStamina( fighterID, opposeStaminaGain );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

CombatInstance::TeamEntry const& CombatInstance::GetTeamRef( TeamID teamID ) const
{
	return mTeams.at( teamID.GetTeamIndex() );
}



CombatInstance::TeamEntry& CombatInstance::GetMutableTeamRef( TeamID teamID )
{
	return mTeams.at( teamID.GetTeamIndex() );
}



CombatInstance::PartyEntry const& CombatInstance::GetPartyRef( PartyID partyID ) const
{
	return mTeams.at( partyID.GetTeamIndex() )
		.mParties.at( partyID.GetPartyIndex() );
}



CombatInstance::PartyEntry& CombatInstance::GetMutablePartyRef( PartyID partyID )
{
	return mTeams.at( partyID.GetTeamIndex() )
		.mParties.at( partyID.GetPartyIndex() );
}



CombatInstance::FighterEntry const& CombatInstance::GetFighterRef( FighterID fighterID ) const
{
	return mTeams.at( fighterID.GetTeamIndex() )
		.mParties.at( fighterID.GetPartyIndex() )
		.mFighters.at( fighterID.GetFighterIndex() );
}



CombatInstance::FighterEntry& CombatInstance::GetMutableFighterRef( FighterID fighterID )
{
	return mTeams.at( fighterID.GetTeamIndex() )
		.mParties.at( fighterID.GetPartyIndex() )
		.mFighters.at( fighterID.GetFighterIndex() );
}



void CombatInstance::LoadFighterFromCharacter( Fighter& fighter, state::ObjectRef const& character )
{
	CombatEngine const& engine = *mCombatEngine;

	// Character
	state::ComponentInstanceRefT<state::comp::Character> const chara =
		character.GetComponentInstanceT<state::comp::Character>();
	RFLOG_TEST_AND_FATAL( chara != nullptr, character, RFCAT_CC3O3, "Missing character component" );
	character::CharData const& charData = chara->mCharData;
	character::Stats const& stats = charData.mStats;

	// TODO: Figure this out
	EntityClass const entityClass = EntityClass::Player;

	fighter.mInnate = element::MakeInnateIdentifier( charData.mInnate );
	fighter.mMaxHealth = engine.LoCalcMaxHealth( stats.mMHealth, entityClass );
	fighter.mCurHealth; // TODO
	fighter.mMaxStamina = kMaxStamina;
	fighter.mCurStamina; // TODO
	fighter.mPhysAtk = stats.mPhysAtk;
	fighter.mPhysDef = stats.mPhysDef;
	fighter.mElemAtk = stats.mElemAtk;
	fighter.mElemDef = stats.mElemDef;
	fighter.mBalance = stats.mBalance;
	fighter.mTechniq = stats.mTechniq;
	fighter.mComboMeter; // TODO
	fighter.mComboTarget; // TODO
}



void CombatInstance::SaveFighterToCharacter( state::MutableObjectRef const& character, Fighter const& fighter ) const
{
	// TODO
}



void CombatInstance::InitializeFighter( Fighter& fighter ) const
{
	fighter.mCurHealth = fighter.mMaxHealth;
	fighter.mCurStamina = fighter.mMaxStamina;
	fighter.mComboMeter = {};
	fighter.mComboTarget = {};
}

///////////////////////////////////////////////////////////////////////////////
}
