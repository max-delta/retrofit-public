#include "stdafx.h"
#include "OverworldCharacter.h"

#include "cc3o3/state/objects/EmptyObject.h"

#include "core_component/ObjectRef.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

component::MutableObjectRef CreateOverworldCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	component::MutableObjectRef const ref = CreateEmptyObject( objIdentifier );
	MakeOverworldCharacterFromDB( ref, charID );
	return ref;
}



void MakeOverworldCharacterFromDB( component::MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}
