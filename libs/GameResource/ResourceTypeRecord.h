#pragma once
#include "project.h"

#include "GameResource/ResourceFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/macros.h"

#include "rftl/string"
#include "rftl/string_view"
#include "rftl/unordered_map"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

class GAMERESOURCE_API ResourceTypeRecord
{
	RF_NO_COPY( ResourceTypeRecord );

	//
	// Forwards
private:
	struct Entry;


	//
	// Types and constants
public:
	using ClassName = rftl::string;
	using ClassInfos = rftl::unordered_map<ClassName, reflect::ClassInfo const*>;

private:
	using Entries = rftl::unordered_map<ClassName, Entry>;


	//
	// Structs
private:
	struct Entry
	{
		reflect::ClassInfo const* mClassInfo = nullptr;
	};


	//
	// Public methods
public:
	explicit ResourceTypeRecord( rftl::string_view displayName );

	// Developer-facing name, mostly used for logging purposes
	rftl::string GetDisplayName() const;

	// Only explicitly registerd classes can be used, other classes will cause
	//  errors when referenced
	// NOTE: Uses class name as registered with RFType
	void RegisterClass( char const* className );
	bool HasClass( char const* className ) const;

	// All ClassInfos that have been registered
	ClassInfos GetClassInfos() const;


	//
	// Private data
private:
	rftl::string const mDisplayName;

	Entries mEntries;
};

///////////////////////////////////////////////////////////////////////////////
}
