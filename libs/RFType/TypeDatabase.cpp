#include "stdafx.h"
#include "TypeDatabase.h"

#include "core/macros.h"
#include "core/meta/LazyInitSingleton.h"
#include "core_rftype/Identifier.h"

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



TypeDatabase& TypeDatabase::GetGlobalMutableInstance()
{
	return GetOrCreateGlobalInstance();
}



TypeDatabase const& TypeDatabase::GetGlobalInstance()
{
	return GetOrCreateGlobalInstance();
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



TypeDatabase& TypeDatabase::GetOrCreateGlobalInstance()
{
	return GetOrInitFunctionStaticScopedSingleton<TypeDatabase>();
}

///////////////////////////////////////////////////////////////////////////////
}
