#include "stdafx.h"
#include "TypeDatabase.h"

#include "core_rftype/ConstructedType.h"
#include "core_rftype/Identifier.h"

#include "core/macros.h"

#include "rftl/memory"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

bool TypeDatabase::RegisterNewClassByName( char const* name, reflect::ClassInfo const& classInfo )
{
	RF_ASSERT( name != nullptr );

	if( IsValidClassName( name ) == false )
	{
		RF_DBGFAIL();
		return false;
	}

	size_t nameLen = strnlen( name, 1024 );
	math::HashVal64 const hash = math::StableHashBytes( name, nameLen );
	bool const registerSuccess = RegisterNewClassByHash( hash, name, classInfo );
	return registerSuccess;
}



reflect::ClassInfo const* TypeDatabase::GetClassInfoByName( char const* name ) const
{
	size_t nameLen = strnlen( name, 1024 );
	math::HashVal64 const hash = math::StableHashBytes( name, nameLen );
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



TypeDatabase::ClassKey TypeDatabase::LookupKeyForClass( reflect::ClassInfo const& classInfo ) const
{
	RF_TODO_ANNOTATION( "Faster reverse lookup table" );

	// For each entry...
	for( ClassInfoByHash::value_type const& entry : mClassInfoByHash )
	{
		if( entry.second.mClassInfo == &classInfo )
		{
			// Return first match
			return ClassKey{ entry.second.mName, entry.first };
		}
	}

	return ClassKey{};
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

bool TypeDatabase::RegisterNewClassByHash( math::HashVal64 const& hash, char const* name, reflect::ClassInfo const& classInfo )
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



bool TypeDatabase::IsValidClassName( char const* name )
{
	return IsValidIdentifier( name );
}

///////////////////////////////////////////////////////////////////////////////
}
