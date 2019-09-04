#pragma once
#include "TypedObjectManager.h"

#include "core/ptr/ptr_transform.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
inline TypedObjectManager<TypeResolver>::TypedObjectManager( ManagerIdentifier identifier, ScopeIdentifier generationScope, TypeResolver&& typeResolver )
	: ObjectManager( identifier, generationScope )
	, mTypeResolver( rftl::move( typeResolver ) )
{
	//
}



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



template<typename TypeResolver>
inline bool TypedObjectManager<TypeResolver>::IsSafeConversion( ComponentRef const& from, ResolvedComponentType to ) const
{
	return GetTypeResolver().operator()( from, to );
}



template<typename TypeResolver>
template<typename ComponentType>
inline bool TypedObjectManager<TypeResolver>::IsValidComponentT( ObjectIdentifier identifier ) const
{
	return IsValidComponent( identifier, ResolveComponentType<ComponentType>() );
}



template<typename TypeResolver>
template<typename ComponentType>
inline ComponentRef TypedObjectManager<TypeResolver>::GetComponentT( ObjectIdentifier identifier ) const
{
	return GetComponent( identifier, ResolveComponentType<ComponentType>() );
}



template<typename TypeResolver>
template<typename ComponentType>
inline MutableComponentRef TypedObjectManager<TypeResolver>::GetMutableComponentT( ObjectIdentifier identifier )
{
	return GetMutableComponentT( identifier, ResolveComponentType<ComponentType>() );
}



template<typename TypeResolver>
template<typename ComponentType>
inline MutableComponentRef TypedObjectManager<TypeResolver>::AddComponentT( ObjectIdentifier identifier, ComponentInstance&& instance )
{
	return AddComponent( identifier, ResolveComponentType<ComponentType>(), rftl::move( instance ) );
}



template<typename TypeResolver>
template<typename ComponentType>
inline bool TypedObjectManager<TypeResolver>::RemoveComponentT( ObjectIdentifier identifier )
{
	return RemoveComponent( identifier, ResolveComponentType<ComponentType>() );
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedObjectManager<TypeResolver>::ComponentInstanceRefT<ComponentType> TypedObjectManager<TypeResolver>::GetComponentInstanceT( ComponentRef const& component ) const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	RF_ASSERT( IsSafeConversion( component, ResolveComponentType<ComponentType>() ) );
	ComponentInstanceRef untyped = GetComponentInstance( component );
	ComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedObjectManager<TypeResolver>::MutableComponentInstanceRefT<ComponentType> TypedObjectManager<TypeResolver>::GetMutableComponentInstanceT( MutableComponentRef const& component )
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	RF_ASSERT( IsSafeConversion( component, ResolveComponentType<ComponentType>() ) );
	MutableComponentInstanceRef untyped = GetMutableComponentInstance( component );
	MutableComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}

///////////////////////////////////////////////////////////////////////////////
}}
