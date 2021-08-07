#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

// WARNING: Expensive linear search
ObjectRef FindObjectByIdentifier( VariableIdentifier const& identifier );
MutableObjectRef FindMutableObjectByIdentifier( VariableIdentifier const& identifier );

///////////////////////////////////////////////////////////////////////////////
}
