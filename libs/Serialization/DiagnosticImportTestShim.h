#pragma once
#include "project.h"

#include "Serialization/SerializationFwd.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

SERIALIZATION_API bool DiagnosticProcessImport( Importer& importer, bool silent );

///////////////////////////////////////////////////////////////////////////////
}
