#pragma once
#include "project.h"

#include "core_math/Hash.h"
#include "core_reflect/ReflectFwd.h"
#include "core_rftype/RFTypeFwd.h"

#include "rftl/functional"
#include "rftl/unordered_map"
#include "rftl/string"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

// Register and keep track of ClassInfo metadata, and some related information
// IMPORTANT: Assumes that ClassInfo references are to static locations in
//  memory, with a pretty heavy leaning towards the assumption that the
//  locations are also in reserved module memory, and not heap-based
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
public:
	using TypeConstructorFunc = rftl::function<ConstructedType()>;
private:
	using ClassInfoByHash = rftl::unordered_map<math::HashVal64, StoredClassInfo, math::DirectHash>;
	using ConstructorByClassInfoAddress = rftl::unordered_map<void const*, TypeConstructorFunc>;


	//
	// Public methods
public:
	bool RegisterNewClassByName( char const* name, reflect::ClassInfo const& classInfo );
	reflect::ClassInfo const* GetClassInfoByName( char const* name ) const;
	reflect::ClassInfo const* GetClassInfoByHash( math::HashVal64 const& hash ) const;

	bool RegisterNewConstructorForClass( TypeConstructorFunc&& constructor, reflect::ClassInfo const& classInfo );
	ConstructedType ConstructClass( reflect::ClassInfo const& classInfo ) const;

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
	ConstructorByClassInfoAddress mConstructorByClassInfoAddress;
};

///////////////////////////////////////////////////////////////////////////////
}
