#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "Rollback/RollbackFwd.h"


// Forwards
namespace RF::cc::state::comp {
class UINavigation;
}

namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateLocalUI(
	rollback::Window& sharedWindow, rollback::Window& privateWindow );
void MakeLocalUI(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref );
WeakPtr<comp::UINavigation> FetchMutableLocalUINavigation();

///////////////////////////////////////////////////////////////////////////////
}
