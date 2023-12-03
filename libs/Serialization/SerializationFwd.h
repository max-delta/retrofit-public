#pragma once
#include "project.h"

#include "core_math/MathFwd.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

namespace exporter {
using InstanceID = uint64_t;
static constexpr InstanceID kInvalidInstanceID = 0;

using TypeID = math::HashVal64;

using IndirectionID = uint64_t;
static constexpr IndirectionID kInvalidIndirectionID = 0;

using ExternalReferenceID = char const*;
}

class ObjectInstance;
class TaggedObjectInstance;

class Importer;
class Exporter;
class ObjectSerializer;

class DiagnosticExporter;
class XmlExporter;
class XmlImporter;

///////////////////////////////////////////////////////////////////////////////
}
