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



TeamIndex CombatInstance::AddTeam()
{
	RF_ASSERT( mTeams.size() < kMaxTeamsPerFight );
	mTeams.emplace_back();
	return math::integer_cast<TeamIndex>( mTeams.size() - 1 );
}



PartyID CombatInstance::AddParty( TeamIndex teamID )
{
	Parties& parties = mTeams.at( teamID ).mParties;
	RF_ASSERT( parties.size() < kMaxPartiesPerTeam );
	parties.emplace_back();
	return { teamID, math::integer_cast<PartyIndex>( parties.size() - 1 ) };
}



CombatantID CombatInstance::AddFighter( PartyID partyID )
{
	Parties& parties = mTeams.at( partyID.mTeam ).mParties;
	Fighters& fighters = parties.at( partyID.mParty ).mFighters;
	RF_ASSERT( fighters.size() < kMaxFightersPerParty );
	fighters.emplace_back();
	return { partyID.mTeam, partyID.mParty, math::integer_cast<FighterIndex>( fighters.size() - 1 ) };
}



void CombatInstance::RemoveFighter( CombatantID combatantID )
{
	Parties& parties = mTeams.at( combatantID.mTeam ).mParties;
	Fighters& fighters = parties.at( combatantID.mParty ).mFighters;
	RF_ASSERT( combatantID.mFighter < fighters.size() );
	fighters.erase( fighters.begin() + combatantID.mFighter );
}



CombatInstance::TeamIDs CombatInstance::GetTeamIDs() const
{
	TeamIDs retVal = {};
	for( TeamIndex i_team = 0; i_team < mTeams.size(); i_team++ )
	{
		retVal.emplace_back( i_team );
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
			retVal.emplace_back( PartyID{ i_team, i_party } );
		}
	}
	return retVal;
}



CombatInstance::CombatantIDs CombatInstance::GetCombatantIDs() const
{
	CombatantIDs retVal = {};
	for( TeamIndex i_team = 0; i_team < mTeams.size(); i_team++ )
	{
		TeamEntry const& team = mTeams.at( i_team );
		for( PartyIndex i_party = 0; i_party < team.mParties.size(); i_party++ )
		{
			PartyEntry const& party = team.mParties.at( i_party );
			for( FighterIndex i_fighter = 0; i_fighter < party.mFighters.size(); i_fighter++ )
			{
				retVal.emplace_back( CombatantID{ i_team, i_party, i_fighter } );
			}
		}
	}
	return retVal;
}



Team CombatInstance::GetTeam( TeamIndex teamID ) const
{
	return GetTeamRef( teamID );
}



Party CombatInstance::GetParty( PartyID partyID ) const
{
	return GetPartyRef( partyID );
}



Combatant CombatInstance::GetCombatant( CombatantID combatantID ) const
{
	return GetCombatantRef( combatantID );
}



void CombatInstance::SetCombatant( CombatantID combatantID, Combatant const& combatant )
{
	Parties& parties = mTeams.at( combatantID.mTeam ).mParties;
	Fighters& fighters = parties.at( combatantID.mParty ).mFighters;
	FighterEntry& fighter = fighters.at( combatantID.mFighter );
	fighter.mCombatant = combatant;
	RF_ASSERT_MSG( fighter.mPersist.IsSet() == false, "Cutting combatant persistence, dubious" );
	fighter.mPersist = {};
}



void CombatInstance::SetCombatant( CombatantID combatantID, component::MutableObjectRef const& character )
{
	Parties& parties = mTeams.at( combatantID.mTeam ).mParties;
	Fighters& fighters = parties.at( combatantID.mParty ).mFighters;
	FighterEntry& fighter = fighters.at( combatantID.mFighter );
	fighter.mPersist = character;

	// HACK: Hard-coded
	// TODO: Load from character
	fighter.mCombatant = {};
	uint32_t genVal = 0;
	fighter.mCombatant.mInnate = mCombatEngine->GenerateRandomInnate( genVal );
	fighter.mCombatant.mMaxHealth = 100;
	fighter.mCombatant.mCurHealth = fighter.mCombatant.mMaxHealth;
	fighter.mCombatant.mMaxStamina = 7;
	fighter.mCombatant.mCurStamina = fighter.mCombatant.mMaxStamina;
	fighter.mCombatant.mPhysAtk = 2;
	fighter.mCombatant.mPhysDef = 2;
	fighter.mCombatant.mElemAtk = 2;
	fighter.mCombatant.mElemDef = 2;
	fighter.mCombatant.mBalance = 2;
	fighter.mCombatant.mTechniq = 2;
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
	return mTeams.at( party.mTeam ).mParties.at( party.mParty ).mParty.mCounterGuage;
}



void CombatInstance::IncreaseCounterGuage( PartyID party, SimVal value )
{
	SimVal& counterGuage = mTeams.at( party.mTeam ).mParties.at( party.mParty ).mParty.mCounterGuage;
	SimVal const maxIncrease = math::integer_cast<SimVal>( kCounterGaugeMax - counterGuage );
	counterGuage = math::Min( maxIncrease, value );
}



AttackProfile CombatInstance::PrepareAttack( CombatantID attackerID, CombatantID defenderID, SimVal attackStrength ) const
{
	Combatant const& attacker = GetCombatantRef( attackerID );
	Combatant const& defender = GetCombatantRef( defenderID );

	AttackProfile retVal = {};

	retVal.mAttackStrength = attackStrength;

	retVal.mNewCombo = true;
	if( attacker.mComboTarget.mTeam == defenderID.mTeam &&
		attacker.mComboTarget.mParty == defenderID.mParty &&
		attacker.mComboTarget.mFighter == defenderID.mFighter )
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



AttackResult CombatInstance::ExecuteAttack( CombatantID attackerID, CombatantID defenderID, SimVal attackStrength )
{
	AttackProfile const profile = PrepareAttack( attackerID, defenderID, attackStrength );
	AttackResult const result = mCombatEngine->HiCalcAttack( profile );

	Combatant& attacker = GetMutableCombatantRef( attackerID );
	Combatant& defender = GetMutableCombatantRef( defenderID );

	attacker.mComboTarget = defenderID;
	attacker.mCurStamina -= profile.mAttackStrength;
	RF_ASSERT( attacker.mCurStamina >= kMinStamina );
	RF_ASSERT( attacker.mCurStamina <= kMaxStamina );
	attacker.mComboMeter = result.mNewComboMeter;
	LargeSimVal const maxDamage = math::integer_cast<LargeSimVal>( defender.mMaxHealth - defender.mCurHealth );
	defender.mCurHealth -= math::Min( result.mDamage, math::integer_truncast<SimVal>( maxDamage ) );
	IncreaseCounterGuage( { defenderID.mTeam, defenderID.mParty }, result.mCoungerGuageIncrease );

	return result;
}

///////////////////////////////////////////////////////////////////////////////

Team const& CombatInstance::GetTeamRef( TeamIndex teamID ) const
{
	return mTeams.at( teamID )
		.mTeam;
}



Team& CombatInstance::GetMutableTeamRef( TeamIndex teamID )
{
	return mTeams.at( teamID )
		.mTeam;
}



Party const& CombatInstance::GetPartyRef( PartyID partyID ) const
{
	return mTeams.at( partyID.mTeam )
		.mParties.at( partyID.mParty )
		.mParty;
}



Party& CombatInstance::GetMutablePartyRef( PartyID partyID )
{
	return mTeams.at( partyID.mTeam )
		.mParties.at( partyID.mParty )
		.mParty;
}



Combatant const& CombatInstance::GetCombatantRef( CombatantID combatantID ) const
{
	return mTeams.at( combatantID.mTeam )
		.mParties.at( combatantID.mParty )
		.mFighters.at( combatantID.mFighter )
		.mCombatant;
}



Combatant& CombatInstance::GetMutableCombatantRef( CombatantID combatantID )
{
	return mTeams.at( combatantID.mTeam )
		.mParties.at( combatantID.mParty )
		.mFighters.at( combatantID.mFighter )
		.mCombatant;
}

///////////////////////////////////////////////////////////////////////////////
}
