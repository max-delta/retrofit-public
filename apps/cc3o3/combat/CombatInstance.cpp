#include "stdafx.h"
#include "CombatInstance.h"

#include "core_math/math_casts.h"


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

///////////////////////////////////////////////////////////////////////////////
}
