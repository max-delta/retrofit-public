#include "stdafx.h"
#include "LocalUI.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/components/UINavigation.h"
#include "cc3o3/state/StateLogging.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateLocalUI(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeLocalUI( sharedWindow, privateWindow, ref );
	return ref;
}



void MakeLocalUI(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref )
{
	// UI Navigation
	{
		MutableComponentInstanceRefT<comp::UINavigation> const navigation =
			ref.AddComponentInstanceT<comp::UINavigation>(
				DefaultCreator<comp::UINavigation>::Create() );
		RFLOG_TEST_AND_FATAL( navigation != nullptr, ref, RFCAT_CC3O3, "Failed to add UI navigation component" );
		navigation->BindToMeta( sharedWindow, privateWindow, ref );
	}

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as local UI" );
}

///////////////////////////////////////////////////////////////////////////////
}
