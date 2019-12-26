#include "stdafx.h"
#include "SiteCharacter.h"

#include "cc3o3/state/objects/EmptyObject.h"

#include "core_component/ObjectRef.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

component::MutableObjectRef CreateSiteCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	component::MutableObjectRef const ref = CreateEmptyObject( objIdentifier );
	MakeSiteCharacterFromDB( ref, charID );
	return ref;
}



void MakeSiteCharacterFromDB( component::MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}
