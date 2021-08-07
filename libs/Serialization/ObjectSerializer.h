#pragma once
#include "project.h"

#include "SerializationFwd.h"

#include "core/macros.h"


// Forwards
namespace RF::reflect {
	struct ClassInfo;
}
namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API ObjectSerializer
{
public:
	RF_NO_INSTANCE( ObjectSerializer );

	static bool SerializeSingleObject( Exporter& exporter, reflect::ClassInfo const& classInfo, void const* classInstance );
};

///////////////////////////////////////////////////////////////////////////////
}
