#pragma once
#include "core_component/ComponentFwd.h"


namespace RF { namespace component {
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

	ObjectIdentifier GetIdentifier() const;

	ComponentRef GetComponent( ResolvedComponentType componentType ) const;


	//
	// Private data
private:
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

	ObjectIdentifier GetIdentifier() const;

	MutableComponentRef GetComponent( ResolvedComponentType componentType ) const;

	// Implicit decay to const ref
	operator ObjectRef() const;


	//
	// Private data
private:
	ObjectManager* mManager = nullptr;
	ObjectIdentifier mIdentifier = kInvalidObjectIdentifier;
};

///////////////////////////////////////////////////////////////////////////////
}}
