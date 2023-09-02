#pragma once
#include "project.h"

#include "SerializationFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/macros.h"

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

		rftl::optional<exporter::TypeID> mTypeID = rftl::nullopt;
		char const* mTypeDebugName = nullptr;
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
