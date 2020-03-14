#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "Rollback/RollbackFwd.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateLocalUI(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier );
void MakeLocalUI(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref );

///////////////////////////////////////////////////////////////////////////////
}
