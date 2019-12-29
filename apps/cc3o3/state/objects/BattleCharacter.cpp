#include "stdafx.h"
#include "BattleCharacter.h"

#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/StateLogging.h"

#include "core_component/TypedObjectRef.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateBattleCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( objIdentifier );
	MakeBattleCharacterFromDB( ref, charID );
	return ref;
}



void MakeBattleCharacterFromDB( MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// TODO

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as battle character" );
}

///////////////////////////////////////////////////////////////////////////////
}
