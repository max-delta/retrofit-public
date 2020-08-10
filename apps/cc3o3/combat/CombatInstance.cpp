#include "stdafx.h"
#include "CombatInstance.h"

#include "cc3o3/combat/Attack.h"
#include "cc3o3/combat/CombatEngine.h"

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
	Parties& parties = mTeams.at( teamID.GetTeamIndex() ).mParties;
	RF_ASSERT( parties.size() < kMaxPartiesPerTeam );
	parties.emplace_back();
	return teamID.GetPartyAt( math::integer_cast<PartyIndex>( parties.size() - 1 ) );
}



FighterID CombatInstance::AddFighter( PartyID partyID )
{
	Parties& parties = mTeams.at( partyID.GetTeamIndex() ).mParties;
	Fighters& fighters = parties.at( partyID.GetPartyIndex() ).mFighters;
	RF_ASSERT( fighters.size() < kMaxFightersPerParty );
	fighters.emplace_back();
	return partyID.GetFighterAt( math::integer_cast<FighterIndex>( fighters.size() - 1 ) );
}



void CombatInstance::RemoveFighter( FighterID fighterID )
{
	Parties& parties = mTeams.at( fighterID.GetTeamIndex() ).mParties;
	Fighters& fighters = parties.at( fighterID.GetPartyIndex() ).mFighters;
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
	TeamEntry const& team = mTeams.at( teamID.GetTeamIndex() );
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
	TeamEntry const& team = mTeams.at( teamID.GetTeamIndex() );
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
	TeamEntry const& team = mTeams.at( partyID.GetTeamIndex() );
	PartyEntry const& party = team.mParties.at( partyID.GetPartyIndex() );
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
	return GetTeamRef( teamID );
}



Party CombatInstance::GetParty( PartyID partyID ) const
{
	return GetPartyRef( partyID );
}



Fighter CombatInstance::GetFighter( FighterID fighterID ) const
{
	return GetFighterRef( fighterID );
}



void CombatInstance::SetCombatant( FighterID fighterID, Fighter const& combatant )
{
	Parties& parties = mTeams.at( fighterID.GetTeamIndex() ).mParties;
	Fighters& fighters = parties.at( fighterID.GetPartyIndex() ).mFighters;
	FighterEntry& fighter = fighters.at( fighterID.GetFighterIndex() );
	fighter.mFighter = combatant;
	RF_ASSERT_MSG( fighter.mPersist.IsSet() == false, "Cutting combatant persistence, dubious" );
	fighter.mPersist = {};
}



void CombatInstance::SetCombatant( FighterID fighterID, component::MutableObjectRef const& character )
{
	Parties& parties = mTeams.at( fighterID.GetTeamIndex() ).mParties;
	Fighters& fighters = parties.at( fighterID.GetPartyIndex() ).mFighters;
	FighterEntry& fighter = fighters.at( fighterID.GetFighterIndex() );
	fighter.mPersist = character;

	// HACK: Hard-coded
	// TODO: Load from character
	fighter.mFighter = {};
	uint32_t genVal = 0;
	fighter.mFighter.mInnate = mCombatEngine->GenerateRandomInnate( genVal );
	fighter.mFighter.mMaxHealth = 100;
	fighter.mFighter.mCurHealth = fighter.mFighter.mMaxHealth;
	fighter.mFighter.mMaxStamina = 7;
	fighter.mFighter.mCurStamina = fighter.mFighter.mMaxStamina;
	fighter.mFighter.mPhysAtk = 2;
	fighter.mFighter.mPhysDef = 2;
	fighter.mFighter.mElemAtk = 2;
	fighter.mFighter.mElemDef = 2;
	fighter.mFighter.mBalance = 2;
	fighter.mFighter.mTechniq = 2;
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
	return mTeams.at( party.GetTeamIndex() ).mParties.at( party.GetPartyIndex() ).mParty.mCounterGuage;
}



void CombatInstance::IncreaseCounterGuage( PartyID party, SimVal value )
{
	SimVal& counterGuage = mTeams.at( party.GetTeamIndex() ).mParties.at( party.GetPartyIndex() ).mParty.mCounterGuage;
	SimVal const maxIncrease = math::integer_cast<SimVal>( kCounterGaugeMax - counterGuage );
	counterGuage = math::Min( maxIncrease, value );
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



AttackProfile CombatInstance::PrepareAttack( FighterID attackerID, FighterID defenderID, SimVal attackStrength ) const
{
	Fighter const& attacker = GetFighterRef( attackerID );
	Fighter const& defender = GetFighterRef( defenderID );

	AttackProfile retVal = {};

	retVal.mAttackStrength = attackStrength;

	retVal.mNewCombo = true;
	if( attacker.mComboTarget == defenderID )
	{
		// TODO: operator==(...) instead
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
	AttackProfile const profile = PrepareAttack( attackerID, defenderID, attackStrength );
	AttackResult const result = mCombatEngine->HiCalcAttack( profile );

	Fighter& attacker = GetMutableFighterRef( attackerID );
	Fighter& defender = GetMutableFighterRef( defenderID );

	attacker.mComboTarget = defenderID;
	attacker.mCurStamina -= profile.mAttackStrength;
	RF_ASSERT( attacker.mCurStamina >= kMinStamina );
	RF_ASSERT( attacker.mCurStamina <= kMaxStamina );
	attacker.mComboMeter = result.mNewComboMeter;
	LargeSimVal const maxDamage = math::integer_cast<LargeSimVal>( defender.mMaxHealth - defender.mCurHealth );
	defender.mCurHealth -= math::Min( result.mDamage, math::integer_truncast<SimVal>( maxDamage ) );
	IncreaseCounterGuage( defenderID.GetParty(), result.mCoungerGuageIncrease );

	return result;
}

///////////////////////////////////////////////////////////////////////////////

Team const& CombatInstance::GetTeamRef( TeamID teamID ) const
{
	return mTeams.at( teamID.GetTeamIndex() )
		.mTeam;
}



Team& CombatInstance::GetMutableTeamRef( TeamID teamID )
{
	return mTeams.at( teamID.GetTeamIndex() )
		.mTeam;
}



Party const& CombatInstance::GetPartyRef( PartyID partyID ) const
{
	return mTeams.at( partyID.GetTeamIndex() )
		.mParties.at( partyID.GetPartyIndex() )
		.mParty;
}



Party& CombatInstance::GetMutablePartyRef( PartyID partyID )
{
	return mTeams.at( partyID.GetTeamIndex() )
		.mParties.at( partyID.GetPartyIndex() )
		.mParty;
}



Fighter const& CombatInstance::GetFighterRef( FighterID fighterID ) const
{
	return mTeams.at( fighterID.GetTeamIndex() )
		.mParties.at( fighterID.GetPartyIndex() )
		.mFighters.at( fighterID.GetFighterIndex() )
		.mFighter;
}



Fighter& CombatInstance::GetMutableFighterRef( FighterID fighterID )
{
	return mTeams.at( fighterID.GetTeamIndex() )
		.mParties.at( fighterID.GetPartyIndex() )
		.mFighters.at( fighterID.GetFighterIndex() )
		.mFighter;
}

///////////////////////////////////////////////////////////////////////////////
}
