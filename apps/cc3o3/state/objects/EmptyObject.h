#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateEmptyObject( state::VariableIdentifier const& objIdentifier );
void MakeEmptyObject( MutableObjectRef const& ref, state::VariableIdentifier const& objIdentifier );

///////////////////////////////////////////////////////////////////////////////
}
