#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "rftl/extension/immutable_string.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

component::MutableObjectRef CreateBattleCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID );
void MakeBattleCharacterFromDB( component::MutableObjectRef const& ref, rftl::immutable_string const& charID );

///////////////////////////////////////////////////////////////////////////////
}
