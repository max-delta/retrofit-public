#pragma once
#include "ResourceLoader.h"

#include "core_rftype/ClassInfoAccessor.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

template<typename ReflectedClass, template<typename> typename Creator>
inline UniquePtr<ReflectedClass> ResourceLoader::LoadClassFromFile(
	ResourceTypeIdentifier typeID,
	file::VFSPath const& path )
{
	using MutableClass = typename rftl::remove_cv<ReflectedClass>::type;
	UniquePtr<MutableClass> classInstance = Creator<MutableClass>::Create();

	bool const popSuccess =
		PopulateClassFromFile(
			typeID,
			*classInstance,
			path );
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
	using MutableClass = typename rftl::remove_cv<ReflectedClass>::type;
	UniquePtr<MutableClass> classInstance = Creator<MutableClass>::Create();

	bool const popSuccess =
		PopulateClassFromBuffer(
			typeID,
			*classInstance,
			buffer );
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
		return false;
	}

	UniquePtr<script::OOLoader> loader = CreateOOLoader();

	InjectTypes( *loader, typeID );
	bool const sourceSuccess = AddSource( *loader, path );
	if( sourceSuccess == false )
	{
		return false;
	}

	return PopulateClassViaOO( *loader, kRootVariableName, classInstance );
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
		return false;
	}

	UniquePtr<script::OOLoader> loader = CreateOOLoader();

	InjectTypes( *loader, typeID );
	bool const sourceSuccess = AddSource( *loader, buffer );
	if( sourceSuccess == false )
	{
		return false;
	}

	return PopulateClassViaOO( *loader, kRootVariableName, classInstance );
}



template<typename ReflectedClass>
inline bool ResourceLoader::PopulateClassViaOO(
	script::OOLoader& loader,
	char const* rootVariableName,
	ReflectedClass& classInstance )
{
	static_assert( rftl::is_const<ReflectedClass>::value == false );
	return PopulateClassViaOO(
		loader,
		rootVariableName,
		rftype::GetClassInfo<ReflectedClass>(),
		&classInstance );
}

///////////////////////////////////////////////////////////////////////////////
}
