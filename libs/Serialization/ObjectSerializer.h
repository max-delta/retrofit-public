#pragma once
#include "project.h"

#include "SerializationFwd.h"


// Forwards
namespace RF { namespace reflect {
	struct ClassInfo;
}}
namespace RF { namespace serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API ObjectSerializer
{
public:
	ObjectSerializer() = delete;

	static bool SerializeSingleObject( Exporter& exporter, reflect::ClassInfo const& classInfo, void const* classInstance );
};

///////////////////////////////////////////////////////////////////////////////
}}
