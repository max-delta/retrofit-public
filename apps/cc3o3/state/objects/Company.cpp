#include "stdafx.h"
#include "Company.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/StateLogging.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateCompany(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeCompany( sharedWindow, privateWindow, ref );
	return ref;
}



void MakeCompany(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref )
{
	// Roster
	{
		MutableComponentInstanceRefT<comp::Roster> const roster =
			ref.AddComponentInstanceT<comp::Roster>(
				DefaultCreator<comp::Roster>::Create() );
		RFLOG_TEST_AND_FATAL( roster != nullptr, ref, RFCAT_CC3O3, "Failed to add roster component" );
		roster->BindToMeta( sharedWindow, privateWindow, ref );
	}

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as company" );
}

///////////////////////////////////////////////////////////////////////////////
}
