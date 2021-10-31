#pragma once
#include "project.h"

#include "cc3o3/state/StateFwd.h"

#include "Rollback/RollbackFwd.h"

#include "rftl/extension/immutable_string.h"


// Forwards
namespace RF::cc::character {
struct CharData;
}

namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateBaseCharacter(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier,
	character::CharData const& charData );
void MakeBaseCharacter(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref,
	character::CharData const& charData );

MutableObjectRef CreateBaseCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier,
	rftl::immutable_string const& charID );
void MakeBaseCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref,
	rftl::immutable_string const& charID );

///////////////////////////////////////////////////////////////////////////////
}
