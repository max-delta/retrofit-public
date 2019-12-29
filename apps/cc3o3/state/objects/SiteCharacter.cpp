#include "stdafx.h"
#include "SiteCharacter.h"

#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/StateLogging.h"

#include "core_component/TypedObjectRef.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateSiteCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeSiteCharacterFromDB( sharedWindow, privateWindow, ref, charID );
	return ref;
}



void MakeSiteCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// TODO

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as site character" );
}

///////////////////////////////////////////////////////////////////////////////
}
