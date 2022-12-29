#include "stdafx.h"
#include "ResourceTypeRegistry.h"

#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

void ResourceTypeRegistry::AddResourceType(
	ResourceTypeIdentifier typeID,
	rftl::string_view displayName )
{
	RF_ASSERT( typeID != kInvalidResourceTypeIdentifier );

	// Create it, making sure there's only one such entry
	bool const newEntry =
		mTypeRecords.emplace( typeID, DefaultCreator<ResourceTypeRecord>::Create( displayName ) ).second;
	RFLOG_TEST_AND_FATAL( newEntry, displayName, RFCAT_GAMERESOURCE,
		"Duplicate type registration for an already existing resource type" );
}



void ResourceTypeRegistry::AddResourceClass(
	ResourceTypeIdentifier typeID,
	char const* className )
{
	RF_ASSERT( typeID != kInvalidResourceTypeIdentifier );

	ResourceTypeRecords::const_iterator const typeRecordIter = mTypeRecords.find( typeID );
	RFLOG_TEST_AND_FATAL( typeRecordIter != mTypeRecords.end(), nullptr, RFCAT_GAMERESOURCE,
		"Unregistered resource type id when attempting to add a new class" );

	RF_ASSERT( typeRecordIter->second != nullptr );
	ResourceTypeRecord& typeRecord = *typeRecordIter->second.Get();

	typeRecord.RegisterClass( className );
}



ResourceTypeRecord::ClassInfos ResourceTypeRegistry::GetClassInfos(
	ResourceTypeIdentifier typeID ) const
{
	RF_ASSERT( typeID != kInvalidResourceTypeIdentifier );

	ResourceTypeRecords::const_iterator const typeRecordIter = mTypeRecords.find( typeID );
	RFLOG_TEST_AND_FATAL( typeRecordIter != mTypeRecords.end(), nullptr, RFCAT_GAMERESOURCE,
		"Unregistered resource type id" );

	RF_ASSERT( typeRecordIter->second != nullptr );
	ResourceTypeRecord const& typeRecord = *typeRecordIter->second.Get();

	return typeRecord.GetClassInfos();
}

///////////////////////////////////////////////////////////////////////////////
}
