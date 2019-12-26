#include "stdafx.h"
#include "BattleCharacter.h"

#include "cc3o3/state/objects/EmptyObject.h"

#include "core_component/ObjectRef.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

component::MutableObjectRef CreateBattleCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	component::MutableObjectRef const ref = CreateEmptyObject( objIdentifier );
	MakeBattleCharacterFromDB( ref, charID );
	return ref;
}



void MakeBattleCharacterFromDB( component::MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}
