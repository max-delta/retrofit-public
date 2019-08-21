#pragma once
#include "core_component/ComponentFwd.h"


namespace RF { namespace component {
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

	ObjectRef GetObject() const;


	//
	// Private data
private:
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

	MutableObjectRef GetObject() const;

	// Implicit decay to const ref
	operator ComponentRef() const;


	//
	// Private data
private:
	ObjectManager* mManager = nullptr;
	ObjectIdentifier mIdentifier = kInvalidObjectIdentifier;
	ResolvedComponentType mComponentType = kInvalidResolvedComponentType;
};

///////////////////////////////////////////////////////////////////////////////
}}
