#pragma once
#include "project.h"

#include "SerializationFwd.h"

#include "core_reflect/ReflectFwd.h"
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
};

///////////////////////////////////////////////////////////////////////////////
}
