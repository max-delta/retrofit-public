#pragma once
#include "project.h"

#include "SerializationFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/macros.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API ObjectDeserializer
{
	RF_NO_INSTANCE( ObjectDeserializer );

public:
	static bool DeserializeSingleObject(
		Importer& importer,
		reflect::ClassInfo const& classInfo,
		void* classInstance );
};

///////////////////////////////////////////////////////////////////////////////
}
