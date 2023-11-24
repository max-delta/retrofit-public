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

///////////////////////////////////////////////////////////////////////////////
}
