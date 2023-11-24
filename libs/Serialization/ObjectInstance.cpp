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
	UniquePtr<void>&& objectStorage )
	: mClassInfo( classInfo )
	, mClassInstance( objectStorage.Get() )
{
	mObjectStorage = rftl::move( objectStorage );
}



ObjectInstance::ObjectInstance(
	ObjectInstance&& rhs )
	: RF_MOVE_CONSTRUCT( mClassInfo )
	, RF_MOVE_CONSTRUCT( mClassInstance )
	, RF_MOVE_CONSTRUCT( mObjectStorage )
{
	RF_MOVE_CLEAR( mObjectStorage );
}

///////////////////////////////////////////////////////////////////////////////
}
