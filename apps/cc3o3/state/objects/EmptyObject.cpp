#include "stdafx.h"
#include "EmptyObject.h"

#include "cc3o3/Common.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Meta.h"

#include "Logging/Logging.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"
#include "core_component/TypedObjectManager.h"
#include "core_state/VariableIdentifier.h"

#include "core/ptr/weak_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateEmptyObject( state::VariableIdentifier const& objIdentifier )
{
	MutableObjectRef const newObj = gObjectManager->AddObject();
	MakeEmptyObject( newObj, objIdentifier );
	return newObj;
}



void MakeEmptyObject( MutableObjectRef const& ref, state::VariableIdentifier const& objIdentifier )
{
	MutableComponentInstanceRefT<comp::Meta> const meta = ref.AddComponentInstanceT<comp::Meta>( DefaultCreator<comp::Meta>::Create( objIdentifier ) );
	RFLOG_TEST_AND_FATAL( meta != nullptr, nullptr, RFCAT_CC3O3, "Failed to added meta component" );
}

///////////////////////////////////////////////////////////////////////////////
}
