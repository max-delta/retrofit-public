#include "stdafx.h"
#include "OverworldCharacter.h"

#include "cc3o3/state/objects/EmptyObject.h"

#include "core_component/TypedObjectRef.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateOverworldCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( objIdentifier );
	MakeOverworldCharacterFromDB( ref, charID );
	return ref;
}



void MakeOverworldCharacterFromDB( MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}
