#pragma once
#include "project.h"

#include "SerializationFwd.h"

// TODO: Refactor to not require ClassInfo in the header
//#include "core_reflect/ReflectFwd.h"
#include "core_reflect/ClassInfo.h"
#include "core_rftype/RFTypeFwd.h"

#include "core/macros.h"

#include "rftl/functional"
#include "rftl/optional"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API ObjectSerializer
{
	RF_NO_INSTANCE( ObjectSerializer );

public:
	struct DeferredIndirection
	{
		exporter::InstanceID mInstanceID = exporter::kInvalidInstanceID;
		reflect::VariableTypeInfo mVariableTypeInfo = {};
		void const* mVariableLocation = nullptr;
		reflect::IndirectionInfo mIndirectionInfo = {};
	};

	struct Params
	{
		exporter::InstanceID mInstanceID = exporter::kInvalidInstanceID;

		// When encountering a class type, identifying information about that
		//  type can be exported, which might be necessary to deserialize it
		//  later when the type information cannot be inferred from just the
		//  context alone
		using TypeLookupSig = rftype::StoredClassKey( reflect::ClassInfo const& );
		using TypeLookupFunc = rftl::function<TypeLookupSig>;
		TypeLookupFunc mTypeLookupFunc = nullptr;

		// If type lookup capability is enabled but fails to find a certain
		//  type, it's likely a sign of error, but serialization can be forced
		//  to continue on with the export anyways
		bool mContinueOnMissingTypeLookups = false;

		// If true, indirections that can be collapsed and serialized inline,
		//  will be, as opposed to be deferred to later serialization
		bool mCollapseAllPossibleIndirections = false;

		// If indirections cause deferrals, they will need to have instance IDs
		//  generated to assign to the indirection, to be able to link them
		//  back together later
		using InstanceIDGenSig = exporter::InstanceID();
		using InstanceIDGenFunc = rftl::function<InstanceIDGenSig>;
		InstanceIDGenFunc mInstanceIDGenerator = nullptr;

		// Some indirections cause deferrals, which will need to be serialized
		//  later somehow for the indirection to be resolved during downstream
		//  deserialization logic
		using IndirectionDeferSig = void( DeferredIndirection const& );
		using IndirectionDeferFunc = rftl::function<IndirectionDeferSig>;
		IndirectionDeferFunc mIndirectionDeferFunc = nullptr;
	};


public:
	static bool SerializeSingleObject(
		Exporter& exporter,
		reflect::ClassInfo const& classInfo,
		void const* classInstance );
	static bool SerializeSingleObject(
		Exporter& exporter,
		reflect::ClassInfo const& classInfo,
		void const* classInstance,
		Params const& params );

	static bool SerializeMultipleObjects(
		Exporter& exporter,
		reflect::ClassInfo const& classInfo,
		void const* classInstance,
		Params const& params );
};

///////////////////////////////////////////////////////////////////////////////
}
