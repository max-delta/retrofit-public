#include "stdafx.h"
#include "ObjectInstance.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

ObjectInstance::ObjectInstance(
	reflect::ClassInfo const& classInfo,
	void* classInstance )
	: mClassInfo( classInfo )
	, mClassInstance( classInstance )
{
	//
}



ObjectInstance::ObjectInstance(
	reflect::ClassInfo const& classInfo,
	WeakPtr<void>&& objectReference )
	: mClassInfo( classInfo )
	, mObjectReference( objectReference )
{
	//
}



ObjectInstance::ObjectInstance(
	reflect::ClassInfo const& classInfo,
	UniquePtr<void>&& objectStorage )
	: mClassInfo( classInfo )
	, mObjectStorage( rftl::move( objectStorage ) )
{
	//
}



ObjectInstance::ObjectInstance(
	ObjectInstance&& rhs )
	: RF_MOVE_CONSTRUCT( mClassInfo )
	, RF_MOVE_CONSTRUCT( mClassInstance )
	, RF_MOVE_CONSTRUCT( mObjectStorage )
{
	RF_MOVE_CLEAR( mObjectReference );
	RF_MOVE_CLEAR( mObjectStorage );
}



void* ObjectInstance::GetStrongestAddress() const
{
	void* retVal = nullptr;

	// Strong
	retVal = mObjectStorage;
	if( retVal != nullptr )
	{
		return retVal;
	}

	// Weak
	retVal = mObjectReference;
	if( retVal != nullptr )
	{
		return retVal;
	}

	// Raw
	retVal = mClassInstance;
	if( retVal != nullptr )
	{
		return retVal;
	}

	return nullptr;
}



bool ObjectInstance::HasStorage() const
{
	return mObjectStorage != nullptr;
}



UniquePtr<void> ObjectInstance::ExtractStorage()
{
	RF_ASSERT( mObjectStorage != nullptr );

	mObjectReference = mObjectStorage;

	return rftl::move( mObjectStorage );
}

///////////////////////////////////////////////////////////////////////////////
}
