#include "stdafx.h"
#include "CompanyManager.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/resource/ResourceLoad.h"
#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Character.h"
#include "cc3o3/state/components/Loadout.h"
#include "cc3o3/state/components/Progression.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/elements/ElementDatabase.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "GameResource/ResourceSaver.h"

#include "RFType/CreateClassInfoDeclaration.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "core_component/TypedObjectRef.h"

#include "rftl/extension/static_vector.h"


namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static file::VFSPath GetLoadoutSavePath( file::VFSPath const& saveRoot, input::PlayerID const& playerID )
{
	return saveRoot.GetChild( "company", rftl::to_string( playerID ), "loadout" );
}

}
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
	return state::VariableIdentifier( "company", rftl::to_string( playerID ) );
}



state::ObjectRef CompanyManager::FindCompanyObject( input::PlayerID const& playerID ) const
{
	return state::FindObjectByIdentifier( FindCompanyIdentifier( playerID ) );
}



rftl::array<state::VariableIdentifier, kRosterSize> CompanyManager::FindRosterIdentifiers( input::PlayerID const& playerID ) const
{
	rftl::array<state::VariableIdentifier, kRosterSize> retVal;

	// Find company object
	state::VariableIdentifier const companyRoot = FindCompanyIdentifier( playerID );
	state::ObjectRef const company = state::FindObjectByIdentifier( companyRoot );
	RFLOG_TEST_AND_FATAL( company.IsSet(), companyRoot, RFCAT_CC3O3, "Failed to find company" );

	// For each roster member...
	state::VariableIdentifier const rosterRoot = companyRoot.GetChild( "member" );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::VariableIdentifier const charRoot = rosterRoot.GetChild( rftl::to_string( i_rosterIndex ) );

		retVal.at( i_rosterIndex ) = charRoot;
	}

	return retVal;
}



rftl::array<state::ObjectRef, kRosterSize> CompanyManager::FindRosterObjects( input::PlayerID const& playerID ) const
{
	rftl::array<state::ObjectRef, kRosterSize> retVal;

	rftl::array<state::VariableIdentifier, kRosterSize> const identifiers = FindRosterIdentifiers( playerID );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::VariableIdentifier const& charRoot = identifiers.at( i_rosterIndex );
		state::ObjectRef const character = state::FindObjectByIdentifier( charRoot );
		retVal.at( i_rosterIndex ) = character;
	}

	return retVal;
}



rftl::array<state::MutableObjectRef, kRosterSize> CompanyManager::FindMutableRosterObjects( input::PlayerID const& playerID ) const
{
	rftl::array<state::MutableObjectRef, kRosterSize> retVal;

	rftl::array<state::VariableIdentifier, kRosterSize> const identifiers = FindRosterIdentifiers( playerID );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::VariableIdentifier const& charRoot = identifiers.at( i_rosterIndex );
		state::MutableObjectRef const character = state::FindMutableObjectByIdentifier( charRoot );
		retVal.at( i_rosterIndex ) = character;
	}

	return retVal;
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
		state::VariableIdentifier const charRoot = rosterRoot.GetChild( rftl::to_string( rosterIndex ) );

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
	if constexpr( config::kDevCheats )
	{
		static bool sDevCheatAllElements = false;
		if( sDevCheatAllElements )
		{
			element::ElementDatabase::ElementIdentifiers const allElements =
				mElementDatabase->GetAllElementIdentifiers();
			ElementCounts cheatVal;
			cheatVal.reserve( cheatVal.size() );
			for( element::ElementIdentifier const& identifier : allElements )
			{
				cheatVal[identifier] = 99;
			}
			return cheatVal;
		}
	}

	// TODO: Additional unlocks from progression
	return mElementDatabase->GetElementsBasedOnTier( progression.mStoryTier );
}



void CompanyManager::UpdateRosterGridMasks( input::PlayerID const& playerID )
{
	RF_ASSERT( playerID != input::kInvalidPlayerID );

	state::ObjectRef const company = FindCompanyObject( playerID );
	RF_ASSERT( company.IsSet() );

	WeakPtr<state::comp::Progression const> const prog = company.GetComponentInstanceT<state::comp::Progression>();
	RF_ASSERT( prog != nullptr );

	// For each roster member...
	rftl::array<state::MutableObjectRef, kRosterSize> const rosterObjects = FindMutableRosterObjects( playerID );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::MutableObjectRef const& character = rosterObjects.at( i_rosterIndex );
		if( character.IsSet() == false )
		{
			continue;
		}

		WeakPtr<state::comp::Loadout> const loadout = character.GetMutableComponentInstanceT<state::comp::Loadout>();
		RF_ASSERT( loadout != nullptr );

		WeakPtr<state::comp::Character const> const chara = character.GetComponentInstanceT<state::comp::Character>();
		RF_ASSERT( chara != nullptr );
		character::Stats const& stats = chara->mCharData.mStats;

		// Calculate and set the grid mask
		loadout->mGridMask = character::GridMask::CalcSlots( stats.mElemPwr, stats.mGridShp, prog->mStoryTier );
	}
}



void CompanyManager::AssignElementToCharacter( state::MutableObjectRef character, character::ElementSlotIndex slot, element::ElementIdentifier element )
{
	RF_ASSERT( character.IsSet() );
	WeakPtr<state::comp::Loadout> const loadout = character.GetMutableComponentInstanceT<state::comp::Loadout>();
	RF_ASSERT( loadout != nullptr );
	AssignElementToCharacter( *loadout, slot, element );
}



void CompanyManager::AssignElementToCharacter( state::comp::Loadout& loadout, character::ElementSlotIndex slot, element::ElementIdentifier element )
{
	loadout.AssignEquippedElement( slot, element );
}



void CompanyManager::ReadLoadoutsFromSave( file::VFSPath const& saveRoot, input::PlayerID const& playerID )
{
	// TODO: Remove
	ReadLegacyLoadoutsFromSave( saveRoot, playerID );

	// TODO: Let newer loader be authoritative
	static constexpr bool kAllowLoadToComplete = false;

	file::VFSPath const loadoutRoot = details::GetLoadoutSavePath( saveRoot, playerID );

	rftl::array<state::MutableObjectRef, kRosterSize> const rosterObjects = FindMutableRosterObjects( playerID );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::MutableObjectRef const& character = rosterObjects.at( i_rosterIndex );
		if( character.IsSet() == false )
		{
			continue;
		}

		WeakPtr<state::comp::Loadout> const loadout = character.GetMutableComponentInstanceT<state::comp::Loadout>();
		RF_ASSERT( loadout != nullptr );

		character::ElementSlots& elements = loadout->mEquippedElements;

		// TODO: BinaryExporter
		// TODO: Figure out a better solution for filename extensions
		static constexpr char kFileExtension[] = ".xml";
		file::VFSPath const loadoutFilePath = loadoutRoot.GetChild( rftl::to_string( i_rosterIndex ) + kFileExtension );

		// Load
		UniquePtr<character::ElementSlots const> loadoutPtr = resource::LoadFromFile<character::ElementSlots const>( loadoutFilePath );
		if( loadoutPtr == nullptr )
		{
			RFLOG_NOTIFY( loadoutFilePath, RFCAT_CC3O3, "Failed to load loadout from file" );
			continue;
		}

		if constexpr( kAllowLoadToComplete )
		{
			RF_TODO_ANNOTATION( "Allow this to actually perform the load" );
			elements = *loadoutPtr;
		}
	}
}



void CompanyManager::WriteLoadoutsToSave( file::VFSPath const& saveRoot, input::PlayerID const& playerID )
{
	// TODO: Remove
	WriteLegacyLoadoutsToSave( saveRoot, playerID );

	file::VFSPath const loadoutRoot = details::GetLoadoutSavePath( saveRoot, playerID );

	rftl::array<state::ObjectRef, kRosterSize> const rosterObjects = FindRosterObjects( playerID );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::ObjectRef const& character = rosterObjects.at( i_rosterIndex );
		if( character.IsSet() == false )
		{
			continue;
		}

		WeakPtr<state::comp::Loadout const> const loadout = character.GetComponentInstanceT<state::comp::Loadout>();
		RF_ASSERT( loadout != nullptr );

		character::ElementSlots const& elements = loadout->mEquippedElements;

		// TODO: BinaryExporter
		// TODO: Figure out a better solution for filename extensions
		static constexpr char kFileExtension[] = ".xml";
		file::VFSPath const loadoutFilePath = loadoutRoot.GetChild( rftl::to_string( i_rosterIndex ) + kFileExtension );

		// Save
		bool const saveSuccess = gResourceSaver->SaveClassToFile(
			rftype::GetClassInfo<character::ElementSlots>(),
			&elements,
			resource::kInvalidResourceTypeIdentifier,
			loadoutFilePath );
		if( saveSuccess == false )
		{
			RFLOG_NOTIFY( loadoutFilePath, RFCAT_CC3O3, "Failed to save loadout to file" );
			continue;
		}
	}
}



void CompanyManager::ReadLegacyLoadoutsFromSave( file::VFSPath const& saveRoot, input::PlayerID const& playerID )
{
	file::VFSPath const loadoutRoot = details::GetLoadoutSavePath( saveRoot, playerID );

	static constexpr size_t kFileSize =
		sizeof( element::ElementIdentifier ) *
		element::kNumElementLevels *
		character::kMaxSlotsPerElementLevel;

	rftl::array<state::MutableObjectRef, kRosterSize> const rosterObjects = FindMutableRosterObjects( playerID );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::MutableObjectRef const& character = rosterObjects.at( i_rosterIndex );
		if( character.IsSet() == false )
		{
			continue;
		}

		WeakPtr<state::comp::Loadout> const loadout = character.GetMutableComponentInstanceT<state::comp::Loadout>();
		RF_ASSERT( loadout != nullptr );

		file::VFSPath const loadoutFilePath = loadoutRoot.GetChild( rftl::to_string( i_rosterIndex ) );
		file::FileHandlePtr const fileHandle = mVfs->GetFileForRead( loadoutFilePath );
		if( fileHandle == nullptr )
		{
			RFLOG_WARNING( loadoutFilePath, RFCAT_CC3O3, "Failed to open loadout for warning, may be a new save?" );
			continue;
		}

		file::FileBuffer const buffer = file::FileBuffer( *fileHandle, false );
		if( buffer.IsEmpty() )
		{
			RFLOG_ERROR( loadoutFilePath, RFCAT_CHAR, "Failed to read to file buffer" );
			continue;
		}

		size_t const bufferSize = buffer.GetSize();
		if( bufferSize != kFileSize )
		{
			RFLOG_NOTIFY( loadoutFilePath, RFCAT_CHAR,
				"Unexpected file size for loadout file, likely corrupt."
				" Loadout will need to be re-created manually in-game." );
			continue;
		}

		// Deserialize
		{
			rftl::byte_view const data = buffer.GetBytes();
			size_t readOffset = 0;
			character::ElementSlots& elements = loadout->mEquippedElements;

			// Level
			for( element::ElementLevel const& level : element::kElementLevels )
			{
				// Slot
				for( size_t i_slot = 0; i_slot < character::kMaxSlotsPerElementLevel; i_slot++ )
				{
					element::ElementBytes bytes = {};
					for( uint8_t& byte : bytes )
					{
						byte = data.at<uint8_t>( readOffset );
						readOffset++;
					}

					character::ElementSlots::Slot& slot = elements.At( { level, i_slot } );
					slot = element::MakeElementIdentifier( bytes );
				}
			}
		}
	}
}



void CompanyManager::WriteLegacyLoadoutsToSave( file::VFSPath const& saveRoot, input::PlayerID const& playerID )
{
	file::VFSPath const loadoutRoot = details::GetLoadoutSavePath( saveRoot, playerID );

	static constexpr size_t kFileSize =
		sizeof( element::ElementIdentifier ) *
		element::kNumElementLevels *
		character::kMaxSlotsPerElementLevel;
	rftl::static_vector<uint8_t, kFileSize> buffer = {};

	rftl::array<state::ObjectRef, kRosterSize> const rosterObjects = FindRosterObjects( playerID );
	for( size_t i_rosterIndex = 0; i_rosterIndex < kRosterSize; i_rosterIndex++ )
	{
		state::ObjectRef const& character = rosterObjects.at( i_rosterIndex );
		if( character.IsSet() == false )
		{
			continue;
		}

		WeakPtr<state::comp::Loadout const> const loadout = character.GetComponentInstanceT<state::comp::Loadout>();
		RF_ASSERT( loadout != nullptr );

		file::VFSPath const loadoutFilePath = loadoutRoot.GetChild( rftl::to_string( i_rosterIndex ) );
		file::FileHandlePtr const fileHandle = mVfs->GetFileForWrite( loadoutFilePath );
		if( fileHandle == nullptr )
		{
			RFLOG_ERROR( loadoutFilePath, RFCAT_CC3O3, "Failed to open loadout for write" );
			continue;
		}

		// Serialize
		buffer.clear();
		{
			character::ElementSlots const& elements = loadout->mEquippedElements;

			// Level
			for( element::ElementLevel const& level : element::kElementLevels )
			{
				// Slot
				for( size_t i_slot = 0; i_slot < character::kMaxSlotsPerElementLevel; i_slot++ )
				{
					element::ElementIdentifier const element = elements.At( { level, i_slot } );
					element::ElementBytes const bytes = element::GetElementBytes( element );
					for( uint8_t const& byte : bytes )
					{
						buffer.emplace_back( byte );
					}
				}
			}
		}
		RF_ASSERT( buffer.capacity() == kFileSize );
		RF_ASSERT( buffer.size() == kFileSize );

		FILE* const file = fileHandle->GetFile();
		RF_ASSERT( file != nullptr );
		size_t const bytesWritten = fwrite( buffer.data(), sizeof( decltype( buffer )::value_type ), buffer.size(), file );
		if( bytesWritten != buffer.size() )
		{
			RFLOG_ERROR( loadoutFilePath, RFCAT_CC3O3, "Failed to write loadout" );
			continue;
		}
	}
}



void CompanyManager::TODO_ValidateLoadouts()
{
	// TODO: Check all active-party characters
	// TODO: Check for valid equipment from a list
	// TODO: Check for valid innate elements from a list
	// TODO: Check for valid equipped elements from a list
	// TODO: Check total active party loadout against company stockpile
}

///////////////////////////////////////////////////////////////////////////////
}
