#include "stdafx.h"
#include "CombatInstance.h"

#include "cc3o3/combat/Attack.h"
#include "cc3o3/combat/CombatEngine.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"


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
	return math::integer_cast<TeamIndex>( mTeams.size() );
}



PartyID CombatInstance::AddParty( TeamIndex teamID )
{
	Parties& parties = mTeams.at( teamID ).mParties;
	RF_ASSERT( parties.size() < kMaxPartiesPerTeam );
	parties.emplace_back();
	return { teamID, math::integer_cast<PartyIndex>( parties.size() ) };
}



CombatantID CombatInstance::AddFighter( PartyID partyID )
{
	Parties& parties = mTeams.at( partyID.mTeam ).mParties;
	Fighters& fighters = parties.at( partyID.mParty ).mFighters;
	RF_ASSERT( fighters.size() < kMaxFightersPerParty );
	fighters.emplace_back();
	return { partyID.mTeam, partyID.mParty, math::integer_cast<FighterIndex>( fighters.size() ) };
}



void CombatInstance::RemoveFighter( CombatantID combatantID )
{
	Parties& parties = mTeams.at( combatantID.mTeam ).mParties;
	Fighters& fighters = parties.at( combatantID.mParty ).mFighters;
	RF_ASSERT( combatantID.mFighter < fighters.size() );
	fighters.erase( fighters.begin() + combatantID.mFighter );
}



Combatant const& CombatInstance::GetCombatantRef( CombatantID combatantID ) const
{
	return mTeams.at( combatantID.mTeam )
		.mParties.at( combatantID.mParty )
		.mFighters.at( combatantID.mFighter )
		.mCombatant;
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
	RF_TODO_BREAK();
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

Combatant& CombatInstance::GetMutableCombatantRef( CombatantID combatantID )
{
	return mTeams.at( combatantID.mTeam )
		.mParties.at( combatantID.mParty )
		.mFighters.at( combatantID.mFighter )
		.mCombatant;
}

///////////////////////////////////////////////////////////////////////////////
}
