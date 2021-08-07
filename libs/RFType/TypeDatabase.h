#pragma once
#include "project.h"

#include "core_math/Hash.h"

#include "rftl/unordered_map"
#include "rftl/string"


// Forwards
namespace RF::reflect {
	struct ClassInfo;
}

namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

class RFTYPE_API TypeDatabase
{
	//
	// Structs
public:
	struct StoredClassInfo
	{
		rftl::string mName;
		reflect::ClassInfo const* mClassInfo;
	};


	//
	// Types
private:
	using ClassInfoByHash = rftl::unordered_map<math::HashVal64, StoredClassInfo, math::DirectHash>;


	//
	// Public methods
public:
	bool RegisterNewClassByName( char const* name, reflect::ClassInfo const& classInfo );
	reflect::ClassInfo const* GetClassInfoByName( char const* name ) const;
	reflect::ClassInfo const* GetClassInfoByHash( math::HashVal64 const& hash ) const;

	static TypeDatabase& GetGlobalMutableInstance();
	static TypeDatabase const& GetGlobalInstance();


	//
	// Private methods
private:
	bool RegisterNewClassByHash( math::HashVal64 const& hash, char const* name, reflect::ClassInfo const& classInfo );

	static bool IsValidClassName( char const* name );
	static TypeDatabase& GetOrCreateGlobalInstance();


	//
	// Private data
private:
	ClassInfoByHash mClassInfoByHash;
};

///////////////////////////////////////////////////////////////////////////////
}
