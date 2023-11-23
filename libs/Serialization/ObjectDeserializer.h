#pragma once
#include "project.h"

#include "SerializationFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/macros.h"

#include "rftl/functional"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API ObjectDeserializer
{
	RF_NO_INSTANCE( ObjectDeserializer );

public:
	struct Params
	{
		// When deserializing a class type, it may have a type identifier,
		//  which in some complex cases is required to instantiate an object
		using TypeLookupSig = reflect::ClassInfo const*( math::HashVal64 const& );
		using TypeLookupFunc = rftl::function<TypeLookupSig>;
		TypeLookupFunc mTypeLookupFunc = nullptr;

		// If type lookup capability is enabled but fails to find a certain
		//  type, it's likely a sign of error, but deserialization can
		//  sometimes be forced to continue on with the import anyways
		bool mContinueOnMissingTypeLookups = false;

		// Indirections are needed for more complex objects, but require extra
		//  machinery to work, including on the serialization side, and so some
		//  cases might not have the data necessary for indirection support
		bool mAllowLocalIndirections = false;
		bool mAllowExternalIndirections = false;
	};


public:
	static bool DeserializeSingleObject(
		Importer& importer,
		reflect::ClassInfo const& classInfo,
		void* classInstance );
	static bool DeserializeSingleObject(
		Importer& importer,
		reflect::ClassInfo const& classInfo,
		void* classInstance,
		Params const& params );
};

///////////////////////////////////////////////////////////////////////////////
}
