#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "Rollback/RollbackFwd.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateCompany(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier );
void MakeCompany(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref );

///////////////////////////////////////////////////////////////////////////////
}
