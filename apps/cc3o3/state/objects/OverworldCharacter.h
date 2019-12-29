#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "Rollback/RollbackFwd.h"

#include "rftl/extension/immutable_string.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateOverworldCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID );
void MakeOverworldCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref, rftl::immutable_string const& charID );

///////////////////////////////////////////////////////////////////////////////
}
