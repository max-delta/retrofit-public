#include "stdafx.h"
#include "TypedObjectRef.h"

#include "core_component/ObjectManager.h"
#include "core_component/ComponentRef.h"


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
inline TypedMutableComponentRef<TypeResolver> TypedMutableObjectRef<TypeResolver>::AddComponentT( ComponentInstance&& instance ) const
{
	ResolvedComponentType const type = mResolver->template operator()<ComponentType>();
	return TypedMutableComponentRef<TypeResolver>( AddComponent( type, rftl::move( instance ) ), mResolver );
}



template<typename TypeResolver>
inline TypedMutableObjectRef<TypeResolver>::operator TypedObjectRef<TypeResolver>() const
{
	return TypedObjectRef<TypeResolver>( MutableObjectRef::operator ObjectRef(), mResolver );
}

///////////////////////////////////////////////////////////////////////////////
}}
