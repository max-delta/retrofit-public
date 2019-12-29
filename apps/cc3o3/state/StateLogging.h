#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "Logging/Logging.h"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

template<>
void WriteContextString( state::VariableIdentifier const& context, Utf8LogContextBuffer& buffer );

template<>
void WriteContextString( cc::state::ObjectRef const& context, Utf8LogContextBuffer& buffer );
template<>
void WriteContextString( cc::state::MutableObjectRef const& context, Utf8LogContextBuffer& buffer );

///////////////////////////////////////////////////////////////////////////////
}
