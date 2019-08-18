#include "stdafx.h"
#include "ObjectManager.h"

#include "core_component/ObjectRef.h"
#include "core_component/ComponentRef.h"
#include "core_component/View.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

ObjectManager::ObjectManager( ManagerIdentifier identifier )
	: mIdentifier( identifier )
{
	RF_ASSERT( identifier != kInvalidManagerIdentifier );
}



ManagerIdentifier ObjectManager::GetManagerIdentifier() const
{
	return mIdentifier;
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

	RF_TODO_BREAK();
	return false;
}



ObjectRef ObjectManager::GetObject( ObjectIdentifier identifier ) const
{
	RF_TODO_BREAK();
	return ObjectRef();
}



ObjectRef ObjectManager::AddObject()
{
	RF_TODO_BREAK();
	return ObjectRef();
}



bool ObjectManager::RemoveObject( ObjectIdentifier identifier )
{
	RF_TODO_BREAK();
	return false;
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

	RF_TODO_BREAK();
	return false;
}



ComponentRef ObjectManager::GetComponent( ObjectIdentifier identifier, ResolvedComponentType componentType ) const
{
	RF_TODO_BREAK();
	return ComponentRef();
}



ComponentRef ObjectManager::AddUninitializedComponent( ObjectIdentifier identifier, ResolvedComponentType componentType )
{
	RF_TODO_BREAK();
	return ComponentRef();
}



bool ObjectManager::RemoveComponent( ObjectIdentifier identifier, ResolvedComponentType componentType )
{
	RF_TODO_BREAK();
	return false;
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



ObjectManager::InternalStateIteration ObjectManager::GetInternalStateIteration() const
{
	return mInternalStateIteration.GetLastGeneratedID();
}

///////////////////////////////////////////////////////////////////////////////
}}
