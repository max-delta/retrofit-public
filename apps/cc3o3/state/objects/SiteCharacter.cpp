#include "stdafx.h"
#include "SiteCharacter.h"

#include "cc3o3/state/objects/EmptyObject.h"

#include "core_component/TypedObjectRef.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateSiteCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( objIdentifier );
	MakeSiteCharacterFromDB( ref, charID );
	return ref;
}



void MakeSiteCharacterFromDB( MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}
