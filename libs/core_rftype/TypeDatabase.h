#pragma once

#include "core_math/Hash.h"
#include "core_reflect/ReflectFwd.h"
#include "core_rftype/RFTypeFwd.h"

#include "rftl/functional"
#include "rftl/unordered_map"
#include "rftl/string"
#include "rftl/string_view"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

// Register and keep track of ClassInfo metadata, and some related information
// IMPORTANT: Assumes that ClassInfo references are to static locations in
//  memory, with a pretty heavy leaning towards the assumption that the
//  locations are also in reserved module memory, and not heap-based
class TypeDatabase
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
	// Classes can be registered by a name, which will produce a unique and
	//  deterministic hash for that name, allowing the same class to be
	//  separately identified across builds / processes / machines / etc
	// NOTE: A class can be registered under multiple names, though it is
	//  discouraged to do this intentionally, since a 1:1 mapping is generally
	//  more intuitive and less error-prone
	bool RegisterNewClassByName( rftl::string_view name, reflect::ClassInfo const& classInfo );
	reflect::ClassInfo const* GetClassInfoByName( rftl::string_view name ) const;
	reflect::ClassInfo const* GetClassInfoByHash( math::HashVal64 const& hash ) const;

	// Lookup a previously registered class
	// NOTE: If a class is registered under multiple keys, one will be chosen
	//  arbitrarily and returned
	// NOTE: Returns an empty key / pointer on failure
	StoredClassKey LookupKeyForClass( reflect::ClassInfo const& classInfo ) const;
	reflect::ClassInfo const* LookupClassForKey( math::HashVal64 const& classKey ) const;

	// Classes can have a constructor registered for them, so that they can
	//  allocated in a reflection-only situation, where the compiler is not
	//  available to perform the necessary construction in-place
	bool RegisterNewConstructorForClass( TypeConstructorFunc&& constructor, reflect::ClassInfo const& classInfo );
	ConstructedType ConstructClass( reflect::ClassInfo const& classInfo ) const;


	//
	// Private methods
private:
	bool RegisterNewClassByHash( math::HashVal64 const& hash, rftl::string_view name, reflect::ClassInfo const& classInfo );

	static bool IsValidClassName( rftl::string_view name );


	//
	// Private data
private:
	ClassInfoByHash mClassInfoByHash;
	ConstructorByClassInfoAddress mConstructorByClassInfoAddress;
};

///////////////////////////////////////////////////////////////////////////////
}
