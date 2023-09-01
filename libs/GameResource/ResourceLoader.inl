#pragma once
#include "ResourceLoader.h"

#include "GameScripting/OOLoader.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

template<typename ReflectedClass, template<typename> typename Creator>
inline UniquePtr<ReflectedClass> ResourceLoader::LoadClassFromFile(
	ResourceTypeIdentifier typeID,
	file::VFSPath const& path )
{
	if( ProbablyAnImporter( path ) )
	{
		// TODO: ObjectDeserializer
		RF_TODO_BREAK();
		return nullptr;
	}

	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, path );
	if( sourceSuccess == false )
	{
		return nullptr;
	}

	using MutableClass = typename rftl::remove_cv<ReflectedClass>::type;
	UniquePtr<MutableClass> classInstance = Creator<MutableClass>::Create();
	bool const popSuccess = loader.PopulateClass( kRootVariableName, *classInstance );
	if( popSuccess == false )
	{
		return nullptr;
	}

	return classInstance;
}



template<typename ReflectedClass, template<typename> typename Creator>
inline UniquePtr<ReflectedClass> ResourceLoader::LoadClassFromBuffer(
	ResourceTypeIdentifier typeID,
	rftl::string_view buffer )
{
	if( ProbablyAnImporter( buffer ) )
	{
		// TODO: ObjectDeserializer
		RF_TODO_BREAK();
		return nullptr;
	}

	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, buffer );
	if( sourceSuccess == false )
	{
		return nullptr;
	}

	using MutableClass = typename rftl::remove_cv<ReflectedClass>::type;
	UniquePtr<MutableClass> classInstance = Creator<MutableClass>::Create();
	bool const popSuccess = loader.PopulateClass( kRootVariableName, *classInstance );
	if( popSuccess == false )
	{
		return nullptr;
	}

	return classInstance;
}



template<typename ReflectedClass>
inline bool ResourceLoader::PopulateClassFromFile(
	ResourceTypeIdentifier typeID,
	ReflectedClass& classInstance,
	file::VFSPath const& path )
{
	if( ProbablyAnImporter( path ) )
	{
		// TODO: ObjectDeserializer
		RF_TODO_BREAK();
		return nullptr;
	}

	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, path );
	if( sourceSuccess == false )
	{
		return false;
	}

	return loader.PopulateClass( kRootVariableName, classInstance );
}



template<typename ReflectedClass>
inline bool ResourceLoader::PopulateClassFromBuffer(
	ResourceTypeIdentifier typeID,
	ReflectedClass& classInstance,
	rftl::string_view buffer )
{
	if( ProbablyAnImporter( buffer ) )
	{
		// TODO: ObjectDeserializer
		RF_TODO_BREAK();
		return nullptr;
	}

	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, buffer );
	if( sourceSuccess == false )
	{
		return false;
	}

	return loader.PopulateClass( kRootVariableName, classInstance );
}

///////////////////////////////////////////////////////////////////////////////
}
