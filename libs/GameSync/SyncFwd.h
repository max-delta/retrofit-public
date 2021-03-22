#pragma once
#include "project.h"

#include "Communication/CommunicationFwd.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

using ConnectionIdentifier = comm::EndpointIdentifier;
static constexpr ConnectionIdentifier kInvalidConnectionIdentifier = {};

struct RollbackInputPack;

///////////////////////////////////////////////////////////////////////////////
}
