#include "stdafx.h"
#include "ResourceTypeRegistry.h"

#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

void ResourceTypeRegistry::AddResourceClass(
	ResourceTypeIdentifier typeID,
	char const* className )
{
	RF_ASSERT( typeID != kInvalidResourceTypeIdentifier );

	// HACK: Lazy register
	// TODO: Require declaration
	{
		if( mTypeRecords.count( typeID ) <= 0 )
		{
			mTypeRecords.emplace( typeID, DefaultCreator<ResourceTypeRecord>::Create( "TODO_Unset" ) );
		}
	}

	ResourceTypeRecord& typeRecord = *mTypeRecords.at( typeID ).Get();
	typeRecord.RegisterClass( className );
}



ResourceTypeRecord::ClassInfos ResourceTypeRegistry::GetClassInfos(
	ResourceTypeIdentifier typeID ) const
{
	RF_ASSERT( typeID != kInvalidResourceTypeIdentifier );

	ResourceTypeRecords::const_iterator const typeRecordIter = mTypeRecords.find( typeID );
	RFLOG_TEST_AND_FATAL( typeRecordIter != mTypeRecords.end(), nullptr, RFCAT_GAMERESOURCE, "Unregistered type id" );

	RF_ASSERT( typeRecordIter->second != nullptr );
	ResourceTypeRecord const& typeRecord = *typeRecordIter->second.Get();

	return typeRecord.GetClassInfos();
}

///////////////////////////////////////////////////////////////////////////////
}
