#include "stdafx.h"
#include "StateHelpers.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Meta.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/Common.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"
#include "core_component/TypedObjectManager.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

ObjectRef FindObjectByIdentifier( VariableIdentifier const& identifier )
{
	return FindMutableObjectByIdentifier( identifier );
}



MutableObjectRef FindMutableObjectByIdentifier( VariableIdentifier const& identifier )
{
	state::ObjectManager& objMan = *gObjectManager;

	// Find all objects with meta information
	rftl::unordered_set<component::ObjectIdentifier> const objects = objMan.FindObjectsT<comp::Meta>();

	// Find the object with the variable identifier we're looking for
	// NOTE: This can be a large list, so callers should cache the found result
	component::ObjectIdentifier found = component::kInvalidObjectIdentifier;
	for( component::ObjectIdentifier const& object : objects )
	{
		MutableComponentInstanceRefT<comp::Meta> const meta =
			objMan.GetMutableComponentT<comp::Meta>( object ).GetMutableComponentInstanceT<comp::Meta>();
		RF_ASSERT( meta != nullptr );

		if( meta->mIdentifier == identifier )
		{
			RFLOG_TEST_AND_FATAL(
				found == component::kInvalidObjectIdentifier,
				identifier,
				RFCAT_CC3O3,
				"Multiple objects found with the same identifier" );
			found = object;
		}
	}

	if( found == component::kInvalidObjectIdentifier )
	{
		return {};
	}

	return objMan.GetMutableObject( found );
}

///////////////////////////////////////////////////////////////////////////////
}
