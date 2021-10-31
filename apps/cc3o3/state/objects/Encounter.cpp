#include "stdafx.h"
#include "Encounter.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/components/Encounter.h"
#include "cc3o3/state/StateLogging.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateEncounter(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeEncounter( sharedWindow, privateWindow, ref );
	return ref;
}



void MakeEncounter(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref )
{
	// Encounter
	{
		MutableComponentInstanceRefT<comp::Encounter> const encounter =
			ref.AddComponentInstanceT<comp::Encounter>(
				DefaultCreator<comp::Encounter>::Create() );
		RFLOG_TEST_AND_FATAL( encounter != nullptr, ref, RFCAT_CC3O3, "Failed to add encounter component" );
		encounter->BindToMeta( sharedWindow, privateWindow, ref );
	}

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as encounter" );
}

///////////////////////////////////////////////////////////////////////////////
}
