#pragma once
#include "core_component/ComponentRef.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: All references are invalid if their parent manager is destroyed,
//  and attempting to use them will result in undefined behavior
template<typename TypeResolver>
class TypedComponentRef : public ComponentRef
{
	//
	// Public methods
public:
	TypedComponentRef() = default;
	TypedComponentRef( ComponentRef const& base, TypeResolver const* resolver );

	TypedObjectRef<TypeResolver> GetObject() const;
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
class TypedMutableComponentRef : public MutableComponentRef
{
	//
	// Public methods
public:
	TypedMutableComponentRef() = default;
	TypedMutableComponentRef( MutableComponentRef const& base, TypeResolver const* resolver );

	TypedMutableObjectRef<TypeResolver> GetObject() const;
	template<typename ComponentType>
	ComponentInstanceRefT<ComponentType> GetComponentInstanceT() const;
	template<typename ComponentType>
	MutableComponentInstanceRefT<ComponentType> GetMutableComponentInstanceT() const;

	// Implicit decay to const ref
	operator TypedComponentRef<TypeResolver>() const;


	//
	// Private data
private:
	TypeResolver const* mResolver = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "TypedComponentRef.inl"
