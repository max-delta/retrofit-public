#include "stdafx.h"
#include "TypedObjectRef.h"

#include "core_component/ObjectManager.h"
#include "core_component/ComponentRef.h"

#include "core/ptr/ptr_transform.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
inline TypedObjectRef<TypeResolver>::TypedObjectRef( ObjectRef const& base, TypeResolver const* resolver )
	: ObjectRef( base )
	, mResolver( resolver )
{
	//
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedComponentRef<TypeResolver> TypedObjectRef<TypeResolver>::GetComponentT() const
{
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	return TypedComponentRef<TypeResolver>( GetComponent( type ), mResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline ComponentInstanceRefT<ComponentType> TypedObjectRef<TypeResolver>::GetComponentInstanceT() const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	ComponentInstanceRef untyped = mManager->GetComponentInstance( GetComponent( type ) );
	ComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}

///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
inline TypedMutableObjectRef<TypeResolver>::TypedMutableObjectRef( MutableObjectRef const& base, TypeResolver const* resolver )
	: MutableObjectRef( base )
	, mResolver( resolver )
{
	//
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedMutableComponentRef<TypeResolver> TypedMutableObjectRef<TypeResolver>::GetComponentT() const
{
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	return TypedMutableComponentRef<TypeResolver>( GetComponent( type ), mResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline ComponentInstanceRefT<ComponentType> TypedMutableObjectRef<TypeResolver>::GetComponentInstanceT() const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	ComponentInstanceRef untyped = mManager->GetComponentInstance( GetComponent( type ) );
	ComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}



template<typename TypeResolver>
template<typename ComponentType>
inline MutableComponentInstanceRefT<ComponentType> TypedMutableObjectRef<TypeResolver>::GetMutableComponentInstanceT() const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	MutableComponentInstanceRef untyped = mManager->GetMutableComponentInstance( GetComponent( type ) );
	MutableComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}



template<typename TypeResolver>
template<typename ComponentType>
inline TypedMutableComponentRef<TypeResolver> TypedMutableObjectRef<TypeResolver>::AddComponentT( ComponentInstanceT<ComponentType>&& instance ) const
{
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	return TypedMutableComponentRef<TypeResolver>( AddComponent( type, rftl::move( instance ) ), mResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline MutableComponentInstanceRefT<ComponentType> TypedMutableObjectRef<TypeResolver>::AddComponentInstanceT( ComponentInstanceT<ComponentType>&& instance ) const
{
	return AddComponentT<ComponentType>( rftl::move( instance ) ).template GetMutableComponentInstanceT<ComponentType>();
}



template<typename TypeResolver>
inline TypedMutableObjectRef<TypeResolver>::operator TypedObjectRef<TypeResolver>() const
{
	return TypedObjectRef<TypeResolver>( MutableObjectRef::operator ObjectRef(), mResolver );
}

///////////////////////////////////////////////////////////////////////////////
}}
