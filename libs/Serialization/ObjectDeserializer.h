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

		// During complex deserialization, objects will often need to be
		// instantiated, which the generic deserializer can't do on its own
		using ClassConstructSig = rftype::ConstructedType( reflect::ClassInfo const& );
		using ClassConstructFunc = rftl::function<ClassConstructSig>;
		ClassConstructFunc mClassConstructFunc = nullptr;

		// When a table of contents (TOC) entry is encountered, preload will
		//  try to construct the objects, but this behavior can be overriden
		// NOTE: An empty return will result in the normal TOC logic
		//  progressing as usual
		using TocInstanceOverrideSig = rftl::optional<ObjectInstance>(
			exporter::InstanceID const&,
			rftl::optional<exporter::TypeID> const& );
		using TocInstanceOverrideFunc = rftl::function<TocInstanceOverrideSig>;
		TocInstanceOverrideFunc mTocInstanceOverrideFunc = nullptr;

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

	static bool DeserializeMultipleObjects(
		Importer& importer,
		int TODO_Output,
		Params const& params );
};

///////////////////////////////////////////////////////////////////////////////
}
