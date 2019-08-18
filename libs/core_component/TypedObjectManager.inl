#pragma once
#include "TypedObjectManager.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
inline TypedObjectManager<TypeResolver>::TypedObjectManager( ManagerIdentifier identifier, TypeResolver&& typeResolver )
	: ObjectManager( identifier )
	, mTypeResolver( rftl::move( typeResolver ) )
{
	//
}



template<typename TypeResolver>
inline TypeResolver const& TypedObjectManager<TypeResolver>::GetTypeResolver() const
{
	return mTypeResolver;
}



template<typename TypeResolver>
template<typename ComponentType>
inline ResolvedComponentType TypedObjectManager<TypeResolver>::ResolveComponentType() const
{
	return GetTypeResolver().operator()<ComponentType>();
}



template<typename TypeResolver>
inline ResolvedComponentType TypedObjectManager<TypeResolver>::ResolveComponentType( ComponentLabel const& label ) const
{
	return GetTypeResolver().operator()( label );
}

///////////////////////////////////////////////////////////////////////////////
}}
