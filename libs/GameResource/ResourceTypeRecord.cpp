#include "stdafx.h"
#include "ResourceTypeRecord.h"

#include "RFType/TypeDatabase.h"
#include "Logging/Logging.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

ResourceTypeRecord::ResourceTypeRecord( rftl::string_view displayName )
	: mDisplayName( displayName )
{
	//
}



rftl::string ResourceTypeRecord::GetDisplayName() const
{
	return mDisplayName;
}



void ResourceTypeRecord::RegisterClass( char const* className )
{
	RF_ASSERT( className != nullptr );

	rftype::TypeDatabase const& typeDatabase = rftype::TypeDatabase::GetGlobalInstance();

	// Make sure it exists
	reflect::ClassInfo const* const classInfo = typeDatabase.GetClassInfoByName( className );
	RFLOG_TEST_AND_FATAL( classInfo != nullptr, mDisplayName, RFCAT_GAMERESOURCE,
		"Could not find class name '%s' when trying to register it with a resource type", className );

	// Register it, making sure there's only one such entry
	bool const newEntry = mEntries.emplace( Entries::value_type{ className, Entry{ classInfo } } ).second;
	RFLOG_TEST_AND_FATAL( newEntry, mDisplayName, RFCAT_GAMERESOURCE,
		"Duplicate class registration for class name '%s'", className );

	RFLOG_TRACE( mDisplayName, RFCAT_GAMERESOURCE, "Registered class name '%s'", className );
}



bool ResourceTypeRecord::HasClass( char const* className ) const
{
	return mEntries.count( className ) > 0;
}



ResourceTypeRecord::ClassInfos ResourceTypeRecord::GetClassInfos() const
{
	ClassInfos retVal;
	retVal.reserve( mEntries.size() );

	// TODO: Some concern about the OOLoader maybe needing dependency classes
	//  to be injected before dependent classes?
	RF_TODO_ANNOTATION( "This list is unordered, does that matter?" );
	for( Entries::value_type const& entry : mEntries )
	{
		RF_ASSERT( entry.second.mClassInfo != nullptr );
		retVal.emplace( entry.first, entry.second.mClassInfo );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
