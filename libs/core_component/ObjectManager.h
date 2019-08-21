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
	static constexpr ScopeIdentifier kDefaultScopeIdentifier = kInvalidScopeIdentifier + 1;

private:
	using ScopedObjectIdentifier = uint32_t;
	using ObjectSet = rftl::unordered_set<ObjectIdentifier>;


	//
	// Public methods
public:
	ObjectManager() = delete;
	ObjectManager( ManagerIdentifier identifier, ScopeIdentifier generationScope );
	ObjectManager( ManagerIdentifier identifier );

	// Manager identifiers used to detect handles being improperly shared
	//  between different managers
	ManagerIdentifier GetManagerIdentifier() const;
	static ManagerIdentifier GetManagerIdentifierFromObjectIdentifier( ObjectIdentifier identifier );

	// Scope identifiers constrain what object identifiers managers are allowed
	//  to generate, allowing managers to safely take responsibility for object
	//  identifiers that they would not have generated on their own
	// NOTE: Intended for use in multiplayer scenarios
	ScopeIdentifier GetGenerationScope() const;
	static ScopeIdentifier GetGenerationScopeFromObjectIdentifier( ObjectIdentifier identifier );

	// Object management
	bool IsValidObject( ObjectIdentifier identifier ) const;
	ObjectRef GetObject( ObjectIdentifier identifier ) const;
	MutableObjectRef GetMutableObject( ObjectIdentifier identifier );
	MutableObjectRef AddObject();
	bool RemoveObject( ObjectIdentifier identifier );

	// Component management
	bool IsValidComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const;
	ComponentRef GetComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const;
	MutableComponentRef GetMutableComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const;
	MutableComponentRef AddUninitializedComponent( ObjectIdentifier identifier, ResolvedComponentType componentType );
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
	// Restricted methods
public:
	struct ConversionHelpers
	{
	private:
		friend class ObjectRef;
		friend class MutableObjectRef;
		friend class ComponentRef;
		friend class MutableComponentRef;

		static ObjectRef CreateObjectRef( MutableObjectRef const& object );
		static ObjectRef CreateObjectRef( ComponentRef const& component );
		static MutableObjectRef CreateMutableObjectRef( MutableComponentRef const& component );

		static ComponentRef CreateComponentRef( MutableComponentRef const& component );
		static ComponentRef CreateComponentRef( ObjectRef const& object, ResolvedComponentType componentType );
		static MutableComponentRef CreateMutableComponentRef( MutableObjectRef const& object, ResolvedComponentType componentType );
	};


	//
	// Private methods
private:
	void RevState();

	static ObjectIdentifier CompositeObjectIdentifier(
		ManagerIdentifier managerIdentifier,
		ScopeIdentifier scopeIdentifier,
		ScopedObjectIdentifier scopedIdentifier );
	ObjectIdentifier GenerateNewObjectIdentifier();

	bool DoesObjectExistInternal( ObjectIdentifier identifier ) const;
	bool CreateNewObjectInternal( ObjectIdentifier identifier );
	bool RemoveExistingObjectInternal( ObjectIdentifier identifier );


	//
	// Private data
private:
	ManagerIdentifier const mManagerIdentifier;
	NonloopingIDGenerator<InternalStateIteration> mInternalStateIteration;

	ScopeIdentifier const mGenerationScope;
	NonloopingIDGenerator<ScopedObjectIdentifier> mObjectIDGenerator;

	ObjectSet mValidObjects;
};

///////////////////////////////////////////////////////////////////////////////
}}
