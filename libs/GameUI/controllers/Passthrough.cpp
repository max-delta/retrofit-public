#include "stdafx.h"
#include "Passthrough.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

ContainerID Passthrough::GetChildContainerID()
{
	return mContainerID;
}



void Passthrough::OnAssign( ContainerManager& manager, Container& container )
{
	mContainerID = manager.CreateChildContainer(
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );
}

///////////////////////////////////////////////////////////////////////////////
}}}
