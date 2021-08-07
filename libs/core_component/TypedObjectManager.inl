#pragma once
#include "TypedObjectManager.h"

#include "core/ptr/ptr_transform.h"


namespace RF::component {
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
inline ResolvedComponentType TypedObjectManager<TypeResolver>::ResolveComponentType( TypeResolver const& resolver )
{
	return resolver.template operator()<ComponentType>();
}



template<typename TypeResolver>
inline ResolvedComponentType TypedObjectManager<TypeResolver>::ResolveComponentType( TypeResolver const& resolver, ComponentLabel const& label )
{
	return resolver.operator()( label );
}



template<typename TypeResolver>
inline bool TypedObjectManager<TypeResolver>::IsSafeConversion( TypeResolver const& resolver, ComponentRef const& from, ResolvedComponentType to )
{
	return resolver.operator()( from, to );
}



template<typename TypeResolver>
inline TypedObjectRef<TypeResolver> TypedObjectManager<TypeResolver>::GetObject( ObjectIdentifier identifier ) const
{
	ObjectRef const temp = ObjectManager::GetObject( identifier );
	return TypedObjectRef<TypeResolver>( temp, &mTypeResolver );
}



template<typename TypeResolver>
inline TypedMutableObjectRef<TypeResolver> TypedObjectManager<TypeResolver>::GetMutableObject( ObjectIdentifier identifier )
{
	MutableObjectRef const temp = ObjectManager::GetMutableObject( identifier );
	return TypedMutableObjectRef<TypeResolver>( temp, &mTypeResolver );
}



template<typename TypeResolver>
inline TypedMutableObjectRef<TypeResolver> TypedObjectManager<TypeResolver>::AddObject()
{
	MutableObjectRef const temp = ObjectManager::AddObject();
	return TypedMutableObjectRef<TypeResolver>( temp, &mTypeResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline bool TypedObjectManager<TypeResolver>::IsValidComponentT( ObjectIdentifier identifier ) const
{
	return IsValidComponent( identifier, ResolveComponentType<ComponentType>( mTypeResolver ) );
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedComponentRef<TypeResolver> TypedObjectManager<TypeResolver>::GetComponentT( ObjectIdentifier identifier ) const
{
	return TypedComponentRef<TypeResolver>( GetComponent( identifier, ResolveComponentType<ComponentType>( mTypeResolver ) ), &mTypeResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedMutableComponentRef<TypeResolver> TypedObjectManager<TypeResolver>::GetMutableComponentT( ObjectIdentifier identifier )
{
	return TypedMutableComponentRef<TypeResolver>( GetMutableComponent( identifier, ResolveComponentType<ComponentType>( mTypeResolver ) ), &mTypeResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedMutableComponentRef<TypeResolver> TypedObjectManager<TypeResolver>::AddComponentT( ObjectIdentifier identifier, ComponentInstance&& instance )
{
	return TypedMutableComponentRef<TypeResolver>( AddComponent( identifier, ResolveComponentType<ComponentType>( mTypeResolver ), rftl::move( instance ) ), &mTypeResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline bool TypedObjectManager<TypeResolver>::RemoveComponentT( ObjectIdentifier identifier )
{
	return RemoveComponent( identifier, ResolveComponentType<ComponentType>( mTypeResolver ) );
}



template<typename TypeResolver>
template<typename ComponentType>
inline ComponentInstanceRefT<ComponentType> TypedObjectManager<TypeResolver>::GetComponentInstanceT( ComponentRef const& component ) const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	RF_ASSERT( IsSafeConversion( mTypeResolver, component, ResolveComponentType<ComponentType>( mTypeResolver ) ) );
	ComponentInstanceRef untyped = GetComponentInstance( component );
	ComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}



template<typename TypeResolver>
template<typename ComponentType>
inline MutableComponentInstanceRefT<ComponentType> TypedObjectManager<TypeResolver>::GetMutableComponentInstanceT( MutableComponentRef const& component )
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	RF_ASSERT( IsSafeConversion( mTypeResolver, component, ResolveComponentType<ComponentType>( mTypeResolver ) ) );
	MutableComponentInstanceRef untyped = GetMutableComponentInstance( component );
	MutableComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}



template<typename TypeResolver>
template<typename ComponentType>
inline rftl::unordered_set<ObjectIdentifier> TypedObjectManager<TypeResolver>::FindObjectsT() const
{
	return FindObjects( ResolveComponentType<ComponentType>( mTypeResolver ) );
}

///////////////////////////////////////////////////////////////////////////////
}
