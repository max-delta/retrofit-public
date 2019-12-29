#include "stdafx.h"
#include "Component.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Meta.h"

#include "core_component/TypedObjectRef.h"


namespace RF::cc::state {
///////////////////////////////////////////////////////////////////////////////

void Component::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	//
}



void Component::BindToMeta( Window& sharedWindow, Window& privateWindow, ObjectRef const& ref )
{
	ComponentInstanceRefT<comp::Meta> const meta = ref.GetComponentInstanceT<comp::Meta>();
	RF_ASSERT( meta != nullptr );
	Bind( sharedWindow, privateWindow, meta->mIdentifier );
}

///////////////////////////////////////////////////////////////////////////////

void NonBindingComponent::Bind( Window& sharedWindow, Window& privateWindow, VariableIdentifier const& parent )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}
