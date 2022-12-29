#pragma once
#include "project.h"

#include "GameResource/ResourceTypeRecord.h"

#include "core/ptr/unique_ptr.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

class GAMERESOURCE_API ResourceTypeRegistry
{
	RF_NO_COPY( ResourceTypeRegistry );

	//
	// Types and constants
private:
	using ResourceTypeRecords = rftl::unordered_map<ResourceTypeIdentifier, UniquePtr<ResourceTypeRecord>>;


	//
	// Public methods
public:
	ResourceTypeRegistry() = default;

	// Only explicitly registerd classes can be used, other classes will cause
	//  errors when referenced
	// NOTE: Uses class name as registered with RFType
	void AddResourceClass(
		ResourceTypeIdentifier typeID,
		char const* className );

	// All ClassInfos that have been registered for a given resource type
	ResourceTypeRecord::ClassInfos GetClassInfos(
		ResourceTypeIdentifier typeID ) const;


	//
	// Private data
private:
	ResourceTypeRecords mTypeRecords;
};

///////////////////////////////////////////////////////////////////////////////
}
