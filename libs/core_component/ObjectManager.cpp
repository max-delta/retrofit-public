#include "stdafx.h"
#include "ObjectManager.h"

#include "core_component/ObjectRef.h"
#include "core_component/ComponentRef.h"
#include "core_component/View.h"

#include "core_math/math_bits.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

ObjectManager::ObjectManager( ManagerIdentifier identifier, ScopeIdentifier generationScope )
	: mManagerIdentifier( identifier )
	, mGenerationScope( generationScope )
{
	RF_ASSERT( identifier != kInvalidManagerIdentifier );
}



ObjectManager::ObjectManager( ManagerIdentifier identifier )
	: ObjectManager( identifier, kDefaultScopeIdentifier )
{
	//
}



ManagerIdentifier ObjectManager::GetManagerIdentifier() const
{
	return mManagerIdentifier;
}



ManagerIdentifier ObjectManager::GetManagerIdentifierFromObjectIdentifier( ObjectIdentifier identifier )
{
	static_assert( sizeof( ObjectIdentifier ) == 8, "Unexpected size" );
	static_assert( sizeof( ManagerIdentifier ) == 2, "Unexpected size" );
	static constexpr size_t kBytesToShift = sizeof( ObjectIdentifier ) - sizeof( ManagerIdentifier );
	static_assert( kBytesToShift == 6, "Bad math" );
	static constexpr size_t kBitsToShift = kBytesToShift * 8;
	static_assert( kBitsToShift == 48, "Bad math" );
	return static_cast<ManagerIdentifier>( identifier >> kBitsToShift );
}



ScopeIdentifier ObjectManager::GetGenerationScope() const
{
	return mGenerationScope;
}



ScopeIdentifier ObjectManager::GetGenerationScopeFromObjectIdentifier( ObjectIdentifier identifier )
{
	static_assert( sizeof( ObjectIdentifier ) == 8, "Unexpected size" );
	static_assert( sizeof( ScopeIdentifier ) == 2, "Unexpected size" );
	static_assert( sizeof( ManagerIdentifier ) == 2, "Unexpected size" );

	static constexpr size_t kBytesToShiftScope = sizeof( ObjectIdentifier ) - ( sizeof( ManagerIdentifier ) + sizeof( ScopeIdentifier ) );
	static_assert( kBytesToShiftScope == 4, "Bad math" );
	static constexpr size_t kBitsToShiftScope = kBytesToShiftScope * 8;
	static_assert( kBitsToShiftScope == 32, "Bad math" );

	ObjectIdentifier const mask = static_cast<ObjectIdentifier>( math::GetAllBitsSet<ScopeIdentifier>() ) << kBitsToShiftScope;
	return static_cast<ScopeIdentifier>( ( identifier & mask ) >> kBitsToShiftScope );
}



bool ObjectManager::IsValidObject( ObjectIdentifier identifier ) const
{
	if( identifier == kInvalidObjectIdentifier )
	{
		return false;
	}

	if( GetManagerIdentifierFromObjectIdentifier( identifier ) != GetManagerIdentifier() )
	{
		// Owned by a different manager
		return false;
	}

	if( DoesObjectExistInternal( identifier ) == false )
	{
		// Doesn't currrently exist
		return false;
	}

	return true;
}



ObjectRef ObjectManager::GetObject( ObjectIdentifier identifier ) const
{
	if( IsValidObject( identifier ) == false )
	{
		return ObjectRef();
	}

	ObjectRef retVal;
	retVal.mManager = this;
	retVal.mIdentifier = identifier;
	return retVal;
}



MutableObjectRef ObjectManager::GetMutableObject( ObjectIdentifier identifier )
{
	if( IsValidObject( identifier ) == false )
	{
		return MutableObjectRef();
	}

	MutableObjectRef retVal;
	retVal.mManager = this;
	retVal.mIdentifier = identifier;
	return retVal;
}



MutableObjectRef ObjectManager::AddObject()
{
	MutableObjectRef retVal;
	retVal.mManager = this;
	retVal.mIdentifier = GenerateNewObjectIdentifier();

	// Newly created id, should never fail
	bool const success = CreateNewObjectInternal( retVal.mIdentifier );
	RF_ASSERT( success );

	return retVal;
}



bool ObjectManager::RemoveObject( ObjectIdentifier identifier )
{
	return RemoveExistingObjectInternal( identifier );
}



bool ObjectManager::IsValidComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const
{
	if( IsValidObject( identifier ) == false )
	{
		return false;
	}

	if( componentType == kInvalidResolvedComponentType )
	{
		return false;
	}

	return DoesComponentExistInternal( identifier, componentType );
}



ComponentRef ObjectManager::GetComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const
{
	if( IsValidComponent( identifier, componentType ) == false )
	{
		return ComponentRef();
	}

	ComponentRef retVal;
	retVal.mManager = this;
	retVal.mIdentifier = identifier;
	retVal.mComponentType = componentType;
	return retVal;
}



MutableComponentRef ObjectManager::GetMutableComponent( ObjectIdentifier identifier, ResolvedComponentType componentType )
{
	if( IsValidComponent( identifier, componentType ) == false )
	{
		return MutableComponentRef();
	}

	MutableComponentRef retVal;
	retVal.mManager = this;
	retVal.mIdentifier = identifier;
	retVal.mComponentType = componentType;
	return retVal;
}



MutableComponentRef ObjectManager::AddComponent( ObjectIdentifier identifier, ResolvedComponentType componentType, ComponentInstance&& instance )
{
	if( IsValidObject( identifier ) == false )
	{
		return MutableComponentRef();
	}

	bool const success = CreateNewComponentInternal( identifier, componentType, rftl::move( instance ) );
	if( success == false )
	{
		return MutableComponentRef();
	}

	MutableComponentRef retVal;
	retVal.mManager = this;
	retVal.mIdentifier = identifier;
	retVal.mComponentType = componentType;
	return retVal;
}



bool ObjectManager::RemoveComponent( ObjectIdentifier identifier, ResolvedComponentType componentType )
{
	return RemoveExistingComponentInternal( identifier, componentType );
}



View ObjectManager::GetView( ResolvedComponentType componentType ) const
{
	RF_TODO_BREAK();
	return View();
}



View ObjectManager::GetUnionView( rftl::unordered_set<ResolvedComponentType>&& componentTypes ) const
{
	RF_TODO_BREAK();
	return View();
}



View ObjectManager::GetIntersectionView( rftl::unordered_set<ResolvedComponentType>&& componentTypes ) const
{
	RF_TODO_BREAK();
	return View();
}



ComponentInstanceRef ObjectManager::GetComponentInstance( ComponentRef const& component ) const
{
	ObjectsByComponentTypeMap::const_iterator const compIter = mComponentManifest.find( component.mComponentType );
	if( compIter != mComponentManifest.end() )
	{
		ComponentInstanceByObjectMap const& objs = compIter->second;
		ComponentInstanceByObjectMap::const_iterator const objIter = objs.find( component.mIdentifier );
		if( objIter != objs.end() )
		{
			return objIter->second;
		}
	}
	return nullptr;
}



MutableComponentInstanceRef ObjectManager::GetMutableComponentInstance( MutableComponentRef const& component )
{
	ObjectsByComponentTypeMap::const_iterator const compIter = mComponentManifest.find( component.mComponentType );
	if( compIter != mComponentManifest.end() )
	{
		ComponentInstanceByObjectMap const& objs = compIter->second;
		ComponentInstanceByObjectMap::const_iterator const objIter = objs.find( component.mIdentifier );
		if( objIter != objs.end() )
		{
			return objIter->second;
		}
	}
	return nullptr;
}



rftl::unordered_set<ObjectIdentifier> ObjectManager::FindObjects( ResolvedComponentType componentType ) const
{
	ObjectsByComponentTypeMap::const_iterator const componentIter = mComponentManifest.find( componentType );
	if( componentIter == mComponentManifest.end() )
	{
		return rftl::unordered_set<ObjectIdentifier>();
	}

	ComponentInstanceByObjectMap const& objects = componentIter->second;
	rftl::unordered_set<ObjectIdentifier> retVal;
	retVal.reserve( objects.size() );
	for( ComponentInstanceByObjectMap::value_type const& objPair : objects )
	{
		ObjectIdentifier const& object = objPair.first;
		retVal.emplace( object );
	}
	return retVal;
}



ObjectManager::InternalStateIteration ObjectManager::GetInternalStateIteration() const
{
	return mInternalStateIteration.GetLastGeneratedID();
}

///////////////////////////////////////////////////////////////////////////////

void ObjectManager::RevState()
{
	mInternalStateIteration.Generate();
}



ObjectIdentifier ObjectManager::CompositeObjectIdentifier(
	ManagerIdentifier managerIdentifier,
	ScopeIdentifier scopeIdentifier,
	ScopedObjectIdentifier scopedIdentifier )
{
	static_assert( sizeof( ObjectIdentifier ) == 8, "Unexpected size" );
	static_assert( sizeof( ScopedObjectIdentifier ) == 4, "Unexpected size" );
	static_assert( sizeof( ScopeIdentifier ) == 2, "Unexpected size" );
	static_assert( sizeof( ManagerIdentifier ) == 2, "Unexpected size" );

	static constexpr size_t kBytesToShiftManager = sizeof( ObjectIdentifier ) - sizeof( ManagerIdentifier );
	static_assert( kBytesToShiftManager == 6, "Bad math" );
	static constexpr size_t kBitsToShiftManager = kBytesToShiftManager * 8;
	static_assert( kBitsToShiftManager == 48, "Bad math" );
	static_assert( sizeof( ScopedObjectIdentifier ) + sizeof( ScopeIdentifier ) <= kBytesToShiftManager, "Unexpected size" );

	static constexpr size_t kBytesToShiftScope = sizeof( ObjectIdentifier ) - ( sizeof( ManagerIdentifier ) + sizeof( ScopeIdentifier ) );
	static_assert( kBytesToShiftScope == 4, "Bad math" );
	static constexpr size_t kBitsToShiftScope = kBytesToShiftScope * 8;
	static_assert( kBitsToShiftScope == 32, "Bad math" );

	return ( static_cast<ObjectIdentifier>( managerIdentifier ) << kBitsToShiftManager ) |
		( static_cast<ObjectIdentifier>( scopeIdentifier ) << kBitsToShiftScope ) |
		scopedIdentifier;
}



ObjectIdentifier ObjectManager::GenerateNewObjectIdentifier()
{
	// NOTE: Intentionally not rev'ing state here, since nothing dangerous
	//  actually changes just from incrementing generation ID

	return CompositeObjectIdentifier( GetManagerIdentifier(), mGenerationScope, mObjectIDGenerator.Generate() );
}



bool ObjectManager::DoesObjectExistInternal( ObjectIdentifier identifier ) const
{
	return mValidObjects.count( identifier ) != 0;
}



bool ObjectManager::CreateNewObjectInternal( ObjectIdentifier identifier )
{
	bool const newAdd = mValidObjects.emplace( identifier ).second;
	RF_ASSERT( newAdd );

	if( newAdd )
	{
		RevState();
		return true;
	}
	return false;
}



bool ObjectManager::RemoveExistingObjectInternal( ObjectIdentifier identifier )
{
	size_t const numRemoved = mValidObjects.erase( identifier );
	bool const wasRemoved = numRemoved != 0;
	RF_ASSERT( wasRemoved );

	if( wasRemoved )
	{
		RevState();
		return true;
	}
	return false;
}



bool ObjectManager::DoesComponentExistInternal( ObjectIdentifier identifier, ResolvedComponentType componentType ) const
{
	// Lookup by type
	ObjectsByComponentTypeMap::const_iterator typeIter = mComponentManifest.find( componentType );
	if( typeIter == mComponentManifest.end() )
	{
		// Never-before-seen component type
		return false;
	}
	ComponentInstanceByObjectMap const& instances = typeIter->second;

	return instances.count( identifier ) != 0;
}



bool ObjectManager::CreateNewComponentInternal( ObjectIdentifier identifier, ResolvedComponentType componentType, ComponentInstance&& instance )
{
	// Lookup by type
	ObjectsByComponentTypeMap::iterator typeIter = mComponentManifest.find( componentType );
	if( typeIter == mComponentManifest.end() )
	{
		// Never-before-seen component type
		typeIter = mComponentManifest.emplace( ObjectsByComponentTypeMap::value_type{ componentType, ComponentInstanceByObjectMap{} } ).first;
		RF_ASSERT( typeIter != mComponentManifest.end() );
		RevState();
	}
	ComponentInstanceByObjectMap& instances = typeIter->second;

	// Attempt insert
	bool const newAdd = instances.emplace( ComponentInstanceByObjectMap::value_type{ identifier, rftl::move( instance ) } ).second;
	if( newAdd == false )
	{
		// Object already had this component
		return false;
	}
	RevState();

	return true;
}



bool ObjectManager::RemoveExistingComponentInternal( ObjectIdentifier identifier, ResolvedComponentType componentType )
{
	// Lookup by type
	ObjectsByComponentTypeMap::iterator typeIter = mComponentManifest.find( componentType );
	if( typeIter == mComponentManifest.end() )
	{
		// Never-before-seen component type
		return false;
	}
	ComponentInstanceByObjectMap& instances = typeIter->second;

	// Attempt remove
	size_t const numRemoved = instances.erase( identifier );
	bool const wasRemoved = numRemoved != 0;
	RF_ASSERT( wasRemoved );
	if( wasRemoved == false )
	{
		return false;
	}
	RevState();

	return true;
}

///////////////////////////////////////////////////////////////////////////////

ObjectRef ObjectManager::ConversionHelpers::CreateObjectRef( MutableObjectRef const& object )
{
	ObjectRef retVal;
	retVal.mManager = object.mManager;
	retVal.mIdentifier = object.mIdentifier;
	return retVal;
}



ObjectRef ObjectManager::ConversionHelpers::CreateObjectRef( ComponentRef const& component )
{
	ObjectRef retVal;
	retVal.mManager = component.mManager;
	retVal.mIdentifier = component.mIdentifier;
	return retVal;
}



MutableObjectRef ObjectManager::ConversionHelpers::CreateMutableObjectRef( MutableComponentRef const& component )
{
	MutableObjectRef retVal;
	retVal.mManager = component.mManager;
	retVal.mIdentifier = component.mIdentifier;
	return retVal;
}



ComponentRef ObjectManager::ConversionHelpers::CreateComponentRef( MutableComponentRef const& component )
{
	ComponentRef retVal;
	retVal.mManager = component.mManager;
	retVal.mIdentifier = component.mIdentifier;
	retVal.mComponentType = component.mComponentType;
	return retVal;
}



ComponentRef ObjectManager::ConversionHelpers::CreateComponentRef( ObjectRef const& object, ResolvedComponentType componentType )
{
	ComponentRef retVal;
	retVal.mManager = object.mManager;
	retVal.mIdentifier = object.mIdentifier;
	retVal.mComponentType = componentType;
	return retVal;
}



MutableComponentRef ObjectManager::ConversionHelpers::CreateMutableComponentRef( MutableObjectRef const& object, ResolvedComponentType componentType )
{
	MutableComponentRef retVal;
	retVal.mManager = object.mManager;
	retVal.mIdentifier = object.mIdentifier;
	retVal.mComponentType = componentType;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

}}
