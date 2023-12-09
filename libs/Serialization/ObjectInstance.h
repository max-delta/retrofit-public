#pragma once
#include "project.h"

#include "SerializationFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/optional"


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
		WeakPtr<void>&& objectReference );
	ObjectInstance(
		reflect::ClassInfo const& classInfo,
		UniquePtr<void>&& objectStorage );

	ObjectInstance(
		ObjectInstance&& rhs );

	// Prefers UniquePtr, then WeakPtr, then raw pointer
	void* GetStrongestAddress() const;

	// Prefers UniquePtr, then WeakPtr
	WeakPtr<void> GetStrongestReference() const;

	bool HasStorage() const;

	// NOTE: Will also cause the the WeakPtr to be set
	UniquePtr<void> ExtractStorage();


public:
	// The class info, to make sense of the void pointers
	reflect::ClassInfo const& mClassInfo;


private:
	// The location, which might be same as the object storage, might be
	//  sourced from a different location, or might be what USED to be in the
	//  object storage but the object storage was migrated elsewhere
	void* const mClassInstance = nullptr;

	// Better than a raw pointer, but might go null
	WeakPtr<void> mObjectReference = nullptr;

	// Root-level instances don't have any place to live on their own, so this
	//  is where they're stored, but they may get pulled out of here and
	//  stored into sub-objects instead if indirections in complex pointer
	//  graphs depend on them, and linkages start to get resolved
	UniquePtr<void> mObjectStorage = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

// Helper wrapper for object instances, that adds identifying information often
//  associated with them
class SERIALIZATION_API TaggedObjectInstance
{
	RF_NO_COPY( TaggedObjectInstance );

public:
	TaggedObjectInstance();
	TaggedObjectInstance(
		TaggedObjectInstance&& rhs );


public:
	// Instance ID, if available
	exporter::InstanceID mInstanceID = exporter::kInvalidInstanceID;

	// Object, if available
	rftl::optional<ObjectInstance> mObject;
};

///////////////////////////////////////////////////////////////////////////////
}
