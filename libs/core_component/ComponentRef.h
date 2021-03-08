#pragma once
#include "core_component/ComponentFwd.h"


namespace RF::component {
///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: All references are invalid if their parent manager is destroyed,
//  and attempting to use them will result in undefined behavior
class ComponentRef
{
	//
	// Friends
private:
	friend class ObjectManager;


	//
	// Public methods
public:
	ComponentRef() = default;

	bool IsSet() const;
	ObjectRef GetObject() const;
	ResolvedComponentType GetComponentType() const;
	ComponentInstanceRef GetComponentInstance() const;

	bool operator==( ComponentRef const& rhs ) const;


	//
	// Protected data
protected:
	ObjectManager const* mManager = nullptr;
	ObjectIdentifier mIdentifier = kInvalidObjectIdentifier;
	ResolvedComponentType mComponentType = kInvalidResolvedComponentType;
};

///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: All references are invalid if their parent manager is destroyed,
//  and attempting to use them will result in undefined behavior
class MutableComponentRef
{
	//
	// Friends
private:
	friend class ObjectManager;


	//
	// Public methods
public:
	MutableComponentRef() = default;

	bool IsSet() const;
	MutableObjectRef GetObject() const;
	ResolvedComponentType GetComponentType() const;
	ComponentInstanceRef GetComponentInstance() const;
	MutableComponentInstanceRef GetMutableComponentInstance() const;

	// Implicit decay to const ref
	operator ComponentRef() const;
	bool operator==( ComponentRef const& rhs ) const;


	//
	// Protected data
protected:
	ObjectManager* mManager = nullptr;
	ObjectIdentifier mIdentifier = kInvalidObjectIdentifier;
	ResolvedComponentType mComponentType = kInvalidResolvedComponentType;
};

///////////////////////////////////////////////////////////////////////////////
}
