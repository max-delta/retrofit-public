#pragma once
#include "project.h"

#include "Communication/CommunicationFwd.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

using ConnectionIdentifier = comm::EndpointIdentifier;
inline constexpr ConnectionIdentifier kInvalidConnectionIdentifier = {};

struct RollbackInputPack;

///////////////////////////////////////////////////////////////////////////////
}
