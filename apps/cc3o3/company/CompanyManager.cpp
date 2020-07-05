#include "stdafx.h"
#include "CompanyManager.h"

#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Character.h"
#include "cc3o3/state/components/Progression.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/elements/ElementDatabase.h"


#include "core_component/TypedObjectRef.h"


namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////

CompanyManager::CompanyManager(
	WeakPtr<file::VFS const> const& vfs,
	WeakPtr<element::ElementDatabase const> const& elementDatabase )
	: mVfs( vfs )
	, mElementDatabase( elementDatabase )
{
	//
}



state::VariableIdentifier CompanyManager::FindCompanyIdentifier( input::PlayerID const& playerID ) const
{
	rftl::string const playerIDAsString = ( rftl::stringstream() << math::integer_cast<size_t>( playerID ) ).str();
	return state::VariableIdentifier( "company", playerIDAsString );
}



state::ObjectRef CompanyManager::FindCompanyObject( input::PlayerID const& playerID ) const
{
	return state::FindObjectByIdentifier( FindCompanyIdentifier( playerID ) );
}



rftl::array<state::VariableIdentifier, kActiveTeamSize> CompanyManager::FindActivePartyIdentifiers( input::PlayerID const& playerID ) const
{
	rftl::array<state::VariableIdentifier, kActiveTeamSize> retVal;

	// Find company object
	state::VariableIdentifier const companyRoot = FindCompanyIdentifier( playerID );
	state::ObjectRef const company = state::FindObjectByIdentifier( companyRoot );
	RFLOG_TEST_AND_FATAL( company.IsSet(), companyRoot, RFCAT_CC3O3, "Failed to find company" );

	// For each active team member...
	state::VariableIdentifier const rosterRoot = companyRoot.GetChild( "member" );
	state::comp::Roster const& roster = *company.GetComponentInstanceT<state::comp::Roster>();
	for( size_t i_teamIndex = 0; i_teamIndex < kActiveTeamSize; i_teamIndex++ )
	{
		RosterIndex const rosterIndex = roster.mActiveTeam.at( i_teamIndex );
		if( rosterIndex == kInvalidRosterIndex )
		{
			// Not active
			continue;
		}
		rftl::string const rosterIndexAsString = ( rftl::stringstream() << math::integer_cast<size_t>( rosterIndex ) ).str();
		state::VariableIdentifier const charRoot = rosterRoot.GetChild( rosterIndexAsString );

		retVal.at( i_teamIndex ) = charRoot;
	}

	return retVal;
}



rftl::array<state::ObjectRef, kActiveTeamSize> CompanyManager::FindActivePartyObjects( input::PlayerID const& playerID ) const
{
	rftl::array<state::ObjectRef, kActiveTeamSize> retVal;

	rftl::array<state::VariableIdentifier, kActiveTeamSize> const identifiers = FindActivePartyIdentifiers( playerID );
	for( size_t i_teamIndex = 0; i_teamIndex < kActiveTeamSize; i_teamIndex++ )
	{
		state::VariableIdentifier const& charRoot = identifiers.at( i_teamIndex );
		if( charRoot.Empty() )
		{
			// Not active
			continue;
		}

		state::ObjectRef const character = state::FindObjectByIdentifier( charRoot );
		RFLOG_TEST_AND_FATAL( character.IsSet(), charRoot, RFCAT_CC3O3, "Failed to find character" );
		retVal.at( i_teamIndex ) = character;
	}

	return retVal;
}



rftl::array<state::MutableObjectRef, kActiveTeamSize> CompanyManager::FindMutableActivePartyObjects( input::PlayerID const& playerID ) const
{
	rftl::array<state::MutableObjectRef, kActiveTeamSize> retVal;

	rftl::array<state::VariableIdentifier, kActiveTeamSize> const identifiers = FindActivePartyIdentifiers( playerID );
	for( size_t i_teamIndex = 0; i_teamIndex < kActiveTeamSize; i_teamIndex++ )
	{
		state::VariableIdentifier const& charRoot = identifiers.at( i_teamIndex );
		if( charRoot.Empty() )
		{
			// Not active
			continue;
		}

		state::MutableObjectRef const character = state::FindMutableObjectByIdentifier( charRoot );
		RFLOG_TEST_AND_FATAL( character.IsSet(), charRoot, RFCAT_CC3O3, "Failed to find character" );
		retVal.at( i_teamIndex ) = character;
	}

	return retVal;
}



state::VariableIdentifier CompanyManager::FindActivePartyCharacterIdentifier( input::PlayerID const& playerID, size_t partyIndex ) const
{
	return FindActivePartyIdentifiers( playerID ).at( partyIndex );
}



state::ObjectRef CompanyManager::FindActivePartyCharacterObject( input::PlayerID const& playerID, size_t partyIndex ) const
{
	state::VariableIdentifier const identifier = FindActivePartyCharacterIdentifier( playerID, partyIndex );
	state::ObjectRef const character = state::FindObjectByIdentifier( identifier );
	RFLOG_TEST_AND_FATAL( character.IsSet(), identifier, RFCAT_CC3O3, "Failed to find character" );
	return character;
}



state::MutableObjectRef CompanyManager::FindMutableActivePartyCharacterObject( input::PlayerID const& playerID, size_t partyIndex ) const
{
	state::VariableIdentifier const identifier = FindActivePartyCharacterIdentifier( playerID, partyIndex );
	state::MutableObjectRef const character = state::FindMutableObjectByIdentifier( identifier );
	RFLOG_TEST_AND_FATAL( character.IsSet(), identifier, RFCAT_CC3O3, "Failed to find character" );
	return character;
}



CompanyManager::ElementCounts CompanyManager::CalcTotalElements( state::ObjectRef const& company ) const
{
	RF_ASSERT( company.IsSet() );
	WeakPtr<state::comp::Progression const> const progression = company.GetComponentInstanceT<state::comp::Progression>();
	RF_ASSERT( progression != nullptr );
	return CalcTotalElements( *progression );
}



CompanyManager::ElementCounts CompanyManager::CalcTotalElements( state::comp::Progression const& progression ) const
{
	// TODO: Additional unlocks from progression
	return mElementDatabase->GetElementsBasedOnTier( progression.mStoryTier );
}



void CompanyManager::AssignElementToCharacter( state::MutableObjectRef character, character::ElementSlotIndex slot, element::ElementIdentifier element )
{
	RF_ASSERT( character.IsSet() );
	WeakPtr<state::comp::Character> const charComp = character.GetMutableComponentInstanceT<state::comp::Character>();
	RF_ASSERT( charComp != nullptr );
	AssignElementToCharacter( *charComp, slot, element );
}



void CompanyManager::AssignElementToCharacter( state::comp::Character& character, character::ElementSlotIndex slot, element::ElementIdentifier element )
{
	character.mCharData.mEquippedElements.At( slot ) = element;
}

///////////////////////////////////////////////////////////////////////////////
}
