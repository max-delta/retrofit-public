#pragma once
#include "core_component/ComponentFwd.h"


namespace RF::component {
///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: All references are invalid if their parent manager is destroyed,
//  and attempting to use them will result in undefined behavior
class ObjectRef
{
	//
	// Friends
private:
	friend class ObjectManager;
	friend class ComponentRef;


	//
	// Public methods
public:
	ObjectRef() = default;

	bool IsSet() const;

	ObjectIdentifier GetIdentifier() const;

	ComponentRef GetComponent( ResolvedComponentType componentType ) const;
	ComponentInstanceRef GetComponentInstance( ResolvedComponentType componentType ) const;

	bool operator==( ObjectRef const& rhs ) const;


	//
	// Protected data
protected:
	ObjectManager const* mManager = nullptr;
	ObjectIdentifier mIdentifier = kInvalidObjectIdentifier;
};

///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: All references are invalid if their parent manager is destroyed,
//  and attempting to use them will result in undefined behavior
class MutableObjectRef
{
	//
	// Friends
private:
	friend class ObjectManager;
	friend class MutableComponentRef;


	//
	// Public methods
public:
	MutableObjectRef() = default;

	bool IsSet() const;

	ObjectIdentifier GetIdentifier() const;

	MutableComponentRef GetComponent( ResolvedComponentType componentType ) const;
	ComponentInstanceRef GetComponentInstance( ResolvedComponentType componentType ) const;
	MutableComponentInstanceRef GetMutableComponentInstance( ResolvedComponentType componentType ) const;
	MutableComponentRef AddComponent( ResolvedComponentType componentType, ComponentInstance&& instance ) const;

	// Implicit decay to const ref
	operator ObjectRef() const;
	bool operator==( ObjectRef const& rhs ) const;


	//
	// Protected data
protected:
	ObjectManager* mManager = nullptr;
	ObjectIdentifier mIdentifier = kInvalidObjectIdentifier;
};

///////////////////////////////////////////////////////////////////////////////
}
