#pragma once
#include "core_component/ObjectRef.h"


namespace RF::component {
///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: All references are invalid if their parent manager is destroyed,
//  and attempting to use them will result in undefined behavior
template<typename TypeResolver>
class TypedObjectRef : public ObjectRef
{
	//
	// Public methods
public:
	TypedObjectRef() = default;
	TypedObjectRef( ObjectRef const& base, TypeResolver const* resolver );

	template<typename ComponentType>
	TypedComponentRef<TypeResolver> GetComponentT() const;
	template<typename ComponentType>
	ComponentInstanceRefT<ComponentType> GetComponentInstanceT() const;


	//
	// Private data
private:
	TypeResolver const* mResolver = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: All references are invalid if their parent manager is destroyed,
//  and attempting to use them will result in undefined behavior
template<typename TypeResolver>
class TypedMutableObjectRef : public MutableObjectRef
{
	//
	// Public methods
public:
	TypedMutableObjectRef() = default;
	TypedMutableObjectRef( MutableObjectRef const& base, TypeResolver const* resolver );

	template<typename ComponentType>
	TypedMutableComponentRef<TypeResolver> GetComponentT() const;
	template<typename ComponentType>
	ComponentInstanceRefT<ComponentType> GetComponentInstanceT() const;
	template<typename ComponentType>
	MutableComponentInstanceRefT<ComponentType> GetMutableComponentInstanceT() const;
	template<typename ComponentType>
	TypedMutableComponentRef<TypeResolver> AddComponentT( ComponentInstanceT<ComponentType>&& instance ) const;
	template<typename ComponentType>
	MutableComponentInstanceRefT<ComponentType> AddComponentInstanceT( ComponentInstanceT<ComponentType>&& instance ) const;

	// Implicit decay to const ref
	operator TypedObjectRef<TypeResolver>() const;


	//
	// Private data
private:
	TypeResolver const* mResolver = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "TypedObjectRef.inl"
