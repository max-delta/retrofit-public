#include "stdafx.h"
#include "TypeDatabase.h"

#include "core/macros.h"
#include "core_rftype/Identifier.h"

#include <memory>


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

bool TypeDatabase::RegisterNewClassByName( char const* name, reflect::ClassInfo const& classInfo )
{
	RF_ASSERT( name != nullptr );

	if( IsValidClassName( name ) == false )
	{
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



TypeDatabase & TypeDatabase::GetGlobalMutableInstance()
{
	return GetOrCreateGlobalInstance();
}



TypeDatabase const & TypeDatabase::GetGlobalInstance()
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



TypeDatabase & TypeDatabase::GetOrCreateGlobalInstance()
{
	#if defined(RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC) && defined(RF_PLATFORM_STRONG_MEMORY_MODEL)
	{
		// What is all this weird shit? Why not just return a reference to the
		//  static variable directly? Well... you need to look at the assembly, but
		//  this reduces some of the heavy-weight function-static thread-safety
		//  code that gets emitted behind the scenes, atleast in VS2017. Not
		//  advisable to do this pattern often, and should be reviewed occasionally
		//  in the assembly to confirm it's working as expected.
		// Some keywords you can look at for some idea about what's going on here:
		//  * Function-local static initialization concurrency
		//  * BSS segment
		//   * Zero page list
		//  * Strong memory model
		//  * L1 cache-line
		//   * Aligned memory access
		//    * Atomic read/write
		RF_ALIGN_ATOMIC_POINTER volatile static TypeDatabase* databasePointer = nullptr;
		if( databasePointer == nullptr )
		{
			static TypeDatabase database = TypeDatabase();

			// This isn't obviously thread-safe since multiple threads could enter
			//  the if, but only one would create the database, and then they would
			//  all write the same value anyways since they're just writing the
			//  address of a link-time-known variable to another link-time-known
			//  variable
			databasePointer = &database;
		}
		return const_cast<TypeDatabase &>( *databasePointer );
	}
	#else
	{
		static TypeDatabase database = TypeDatabase();
		return database;
	}
	#endif
}

///////////////////////////////////////////////////////////////////////////////
}}
