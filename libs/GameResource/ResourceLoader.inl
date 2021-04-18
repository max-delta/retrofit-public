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
	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, path );
	if( sourceSuccess == false )
	{
		return nullptr;
	}

	UniquePtr<ReflectedClass> classInstance = Creator<ReflectedClass>::Create();
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
	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, buffer );
	if( sourceSuccess == false )
	{
		return nullptr;
	}

	UniquePtr<ReflectedClass> classInstance = Creator<ReflectedClass>::Create();
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
