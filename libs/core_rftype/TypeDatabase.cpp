#include "stdafx.h"
#include "TypeDatabase.h"

#include "core_rftype/ConstructedType.h"
#include "core_rftype/Identifier.h"
#include "core_rftype/StoredClassKey.h"

#include "core/macros.h"

#include "rftl/memory"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

bool TypeDatabase::RegisterNewClassByName( rftl::string_view name, reflect::ClassInfo const& classInfo )
{
	RF_ASSERT( name.empty() == false );

	if( IsValidClassName( name ) == false )
	{
		RF_DBGFAIL();
		return false;
	}

	math::HashVal64 const hash = math::StableHashBytes( name.data(), name.size() );
	bool const registerSuccess = RegisterNewClassByHash( hash, name, classInfo );
	return registerSuccess;
}



reflect::ClassInfo const* TypeDatabase::GetClassInfoByName( rftl::string_view name ) const
{
	math::HashVal64 const hash = math::StableHashBytes( name.data(), name.size() );
	return GetClassInfoByHash( hash );
}



reflect::ClassInfo const* TypeDatabase::GetClassInfoByHash( math::HashVal64 const& hash ) const
{
	ClassInfoByHash::const_iterator const iter = mClassInfoByHash.find( hash );
	if( iter == mClassInfoByHash.end() )
	{
		return nullptr;
	}

	RF_ASSERT( iter->second.mClassInfo != nullptr );
	return iter->second.mClassInfo;
}



StoredClassKey TypeDatabase::LookupKeyForClass( reflect::ClassInfo const& classInfo ) const
{
	RF_TODO_ANNOTATION( "Faster reverse lookup table" );

	// For each entry...
	for( ClassInfoByHash::value_type const& entry : mClassInfoByHash )
	{
		if( entry.second.mClassInfo == &classInfo )
		{
			// Return first match
			return StoredClassKey( entry.second.mName, entry.first );
		}
	}

	return StoredClassKey{};
}


reflect::ClassInfo const* TypeDatabase::LookupClassForKey( math::HashVal64 const& classKey ) const
{
	// Lookup by key
	ClassInfoByHash::const_iterator const iter = mClassInfoByHash.find( classKey );
	if( iter == mClassInfoByHash.end() )
	{
		return nullptr;
	}

	return iter->second.mClassInfo;
}



bool TypeDatabase::RegisterNewConstructorForClass( TypeConstructorFunc&& constructor, reflect::ClassInfo const& classInfo )
{
	void const* const address = &classInfo;

	if( constructor == nullptr )
	{
		return false;
	}

	if( mConstructorByClassInfoAddress.count( address ) != 0 )
	{
		return false;
	}

	mConstructorByClassInfoAddress[address] = rftl::move( constructor );
	return true;
}



ConstructedType TypeDatabase::ConstructClass( reflect::ClassInfo const& classInfo ) const
{
	void const* const address = &classInfo;

	ConstructorByClassInfoAddress::const_iterator const iter = mConstructorByClassInfoAddress.find( address );
	if( iter == mConstructorByClassInfoAddress.end() )
	{
		return {};
	}

	RF_ASSERT( iter->second != nullptr );
	return iter->second();
}

///////////////////////////////////////////////////////////////////////////////

bool TypeDatabase::RegisterNewClassByHash( math::HashVal64 const& hash, rftl::string_view name, reflect::ClassInfo const& classInfo )
{
	if( mClassInfoByHash.count( hash ) != 0 )
	{
		return false;
	}

	StoredClassInfo& newStorage = mClassInfoByHash[hash];
	newStorage.mName = name;
	newStorage.mClassInfo = &classInfo;
	return true;
}



bool TypeDatabase::IsValidClassName( rftl::string_view name )
{
	return IsValidIdentifier( name );
}

///////////////////////////////////////////////////////////////////////////////
}
