#include "stdafx.h"
#include "TypedComponentRef.h"

#include "core_component/TypedObjectManager.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
inline TypedComponentRef<TypeResolver>::TypedComponentRef( ComponentRef const& base, TypeResolver const* resolver )
	: ComponentRef( base )
	, mResolver( resolver )
{
	//
}



template<typename TypeResolver>
inline TypedObjectRef<TypeResolver> TypedComponentRef<TypeResolver>::GetObject() const
{
	return TypedObjectRef<TypeResolver>( ComponentRef::GetObject(), mResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline ComponentInstanceRefT<ComponentType> TypedComponentRef<TypeResolver>::GetComponentInstanceT() const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	RF_ASSERT( TypedObjectManager<TypeResolver>::IsSafeConversion( *mResolver, *this, type ) );
	ComponentInstanceRef untyped = mManager->GetComponentInstance( *this );
	ComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}

///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
inline TypedMutableComponentRef<TypeResolver>::TypedMutableComponentRef( MutableComponentRef const& base, TypeResolver const* resolver )
	: MutableComponentRef( base )
	, mResolver( resolver )
{
	//
}



template<typename TypeResolver>
inline TypedMutableObjectRef<TypeResolver> TypedMutableComponentRef<TypeResolver>::GetObject() const
{
	return TypedMutableObjectRef<TypeResolver>( MutableComponentRef::GetObject(), mResolver );
}



template<typename TypeResolver>
template<typename ComponentType>
inline ComponentInstanceRefT<ComponentType> TypedMutableComponentRef<TypeResolver>::GetComponentInstanceT() const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	RF_ASSERT( TypedObjectManager<TypeResolver>::IsSafeConversion( *mResolver, *this, type ) );
	ComponentInstanceRef untyped = mManager->GetComponentInstance( *this );
	ComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}



template<typename TypeResolver>
template<typename ComponentType>
inline MutableComponentInstanceRefT<ComponentType> TypedMutableComponentRef<TypeResolver>::GetMutableComponentInstanceT() const
{
	// This is dangerous! Make sure your TypeResolver is consistent!
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	RF_ASSERT( TypedObjectManager<TypeResolver>::IsSafeConversion( *mResolver, *this, type ) );
	MutableComponentInstanceRef untyped = mManager->GetMutableComponentInstance( *this );
	MutableComponentInstanceRefT<ComponentType> typed;
	PtrTransformer<ComponentType>::PerformNonTypesafeTransformation( rftl::move( untyped ), typed );
	return typed;
}



template<typename TypeResolver>
inline TypedMutableComponentRef<TypeResolver>::operator TypedComponentRef<TypeResolver>() const
{
	return TypedComponentRef<TypeResolver>( MutableComponentRef::operator ComponentRef(), mResolver );
}

///////////////////////////////////////////////////////////////////////////////
}}
