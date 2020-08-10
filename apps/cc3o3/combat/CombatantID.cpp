#include "stdafx.h"
#include "CombatantID.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////
namespace details {

TeamID CombatantID::GetTeam() const
{
	TeamID retVal;
	retVal.mValue = math::SnapNearest( mValue, kTeamStride );
	return retVal;
}



PartyID CombatantID::GetParty() const
{
	PartyID retVal;
	retVal.mValue = math::SnapNearest( mValue, kPartyStride );
	return retVal;
}



FighterID CombatantID::GetFighter() const
{
	static_assert( kFighterStride == 1 );
	FighterID retVal;
	retVal.mValue = mValue;
	return retVal;
}



PartyID CombatantID::GetPartyAt( PartyIndex index ) const
{
	RF_ASSERT( index < kMaxPartiesPerTeam );
	PartyID retVal;
	retVal.mValue = math::integer_cast<Value>( GetTeam().mValue + index * kPartyStride );
	return retVal;
}



FighterID CombatantID::GetFighterAt( FighterIndex index ) const
{
	RF_ASSERT( index < kMaxFightersPerParty );
	FighterID retVal;
	retVal.mValue = math::integer_cast<Value>( GetParty().mValue + index * kFighterStride );
	return retVal;
}



TeamIndex CombatantID::GetTeamIndex() const
{
	return math::integer_cast<Value>( GetTeam().mValue / kTeamStride );
}



PartyIndex CombatantID::GetPartyIndex() const
{
	return math::integer_cast<Value>( ( GetParty().mValue % kTeamStride ) / kPartyStride );
}



FighterIndex CombatantID::GetFighterIndex() const
{
	return math::integer_cast<Value>( ( ( GetFighter().mValue % kTeamStride ) % kPartyStride ) / kFighterStride );
}

}
///////////////////////////////////////////////////////////////////////////////
namespace details {

CombatantID::CombatantID( Value value )
	: mValue( value )
{
	//
}

}
///////////////////////////////////////////////////////////////////////////////

TeamID TeamID::MakeTeam( TeamIndex team )
{
	RF_ASSERT( team < kMaxTeamsPerFight );
	TeamID retVal = {};
	retVal.mValue = math::integer_cast<Value>( team * kTeamStride );
	return retVal;
}

bool TeamID::operator==( TeamID const& rhs ) const
{
	return mValue == rhs.mValue;
}

bool TeamID::operator!=( TeamID const& rhs ) const
{
	return !operator==( rhs );
}

///////////////////////////////////////////////////////////////////////////////

PartyID PartyID::MakeParty( TeamIndex team, PartyIndex party )
{
	return TeamID::MakeTeam( team ).GetPartyAt( party );
}

bool PartyID::operator==( PartyID const& rhs ) const
{
	return mValue == rhs.mValue;
}

bool PartyID::operator!=( PartyID const& rhs ) const
{
	return !operator==( rhs );
}

///////////////////////////////////////////////////////////////////////////////

FighterID FighterID::MakeFighter( TeamIndex team, PartyIndex party, FighterIndex fighter )
{
	return TeamID::MakeTeam( team ).GetPartyAt( party ).GetFighterAt( fighter );
}

bool FighterID::operator==( FighterID const& rhs ) const
{
	return mValue == rhs.mValue;
}

bool FighterID::operator!=( FighterID const& rhs ) const
{
	return !operator==( rhs );
}

///////////////////////////////////////////////////////////////////////////////
}
