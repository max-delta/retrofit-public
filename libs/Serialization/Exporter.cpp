#include "stdafx.h"
#include "Exporter.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

Exporter::Exporter() = default;
Exporter::~Exporter() = default;



bool Exporter::Property_AddDebugTypeIDAttribute( TypeID const& typeID, char const* debugName )
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
