#include "stdafx.h"
#include "Passthrough.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::Passthrough )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::Passthrough );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

ContainerID Passthrough::GetChildContainerID() const
{
	return mChildContainerID;
}



void Passthrough::OnInstanceAssign( UIContext& context, Container& container )
{
	mChildContainerID = CreateChildContainer(
		context.GetMutableContainerManager(),
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );
}

///////////////////////////////////////////////////////////////////////////////
}
