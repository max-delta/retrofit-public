#pragma once
#include "project.h"

#include "SerializationFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

// An instance of an object, primarily for use during deserialization, where
//  complex deserialization may result in multiple objects that need to be
//  tracked and managed
class SERIALIZATION_API ObjectInstance
{
	RF_NO_COPY( ObjectInstance );

public:
	ObjectInstance(
		reflect::ClassInfo const& classInfo,
		void* classInstance );
	ObjectInstance(
		reflect::ClassInfo const& classInfo,
		UniquePtr<void>&& objectStorage );

	ObjectInstance(
		ObjectInstance&& rhs );


public:
	// The class info, to make sense of the void pointers
	reflect::ClassInfo const& mClassInfo;

	// The location, which might be same as the object storage, might be
	//  sourced from a different location, or might be what USED to be in the
	//  object storage but the object storage was migrated elsewhere
	void* const mClassInstance;

	// Root-level instances don't have any place to live on their own, so this
	//  is where they're stored, but they may get pulled out of here and
	//  stored into sub-objects instead if indirections in complex pointer
	//  graphs depend on them, and linkages start to get resolved
	UniquePtr<void> mObjectStorage = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
