#pragma once
#include "core_component/ComponentFwd.h"

#include "core/idgen.h"

#include "rftl/unordered_set"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

class ObjectManager
{
	RF_NO_COPY( ObjectManager );

	//
	// Types
public:
	using InternalStateIteration = uint64_t;


	//
	// Public methods
public:
	ObjectManager( ManagerIdentifier identifier );

	// Manager identifiers used to detect handles being improperly shared
	//  between different managers
	ManagerIdentifier GetManagerIdentifier() const;
	static ManagerIdentifier GetManagerIdentifierFromObjectIdentifier( ObjectIdentifier identifier );

	// Object management
	bool IsValidObject( ObjectIdentifier identifier ) const;
	ObjectRef GetObject( ObjectIdentifier identifier ) const;
	ObjectRef AddObject();
	bool RemoveObject( ObjectIdentifier identifier );

	// Component management
	bool IsValidComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const;
	ComponentRef GetComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const;
	ComponentRef AddUninitializedComponent( ObjectIdentifier identifier, ResolvedComponentType componentType );
	bool RemoveComponent( ObjectIdentifier identifier, ResolvedComponentType componentType );

	// View management
	View GetView( ResolvedComponentType componentType ) const;
	View GetUnionView( rftl::unordered_set<ResolvedComponentType>&& componentTypes ) const;
	View GetIntersectionView( rftl::unordered_set<ResolvedComponentType>&& componentTypes ) const;

	// Adding/removing items will cause the state to iterate, which can be used
	//  to detect unsafe multi-threaded operations, or modifying while
	//  iterating in a single-threaded operation
	InternalStateIteration GetInternalStateIteration() const;


	//
	// Private data
private:
	ManagerIdentifier const mIdentifier;
	NonloopingIDGenerator<InternalStateIteration> mInternalStateIteration;
};

///////////////////////////////////////////////////////////////////////////////
}}
