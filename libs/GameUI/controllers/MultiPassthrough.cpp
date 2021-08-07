#include "stdafx.h"
#include "MultiPassthrough.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::MultiPassthrough )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::MultiPassthrough );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

MultiPassthrough::MultiPassthrough( size_t numChildren )
{
	mChildContainerIDs.resize( numChildren, kInvalidContainerID );
}



ContainerID MultiPassthrough::GetChildContainerID( size_t index ) const
{
	RF_ASSERT( index < mChildContainerIDs.size() );
	return mChildContainerIDs.at( index );
}



void MultiPassthrough::OnInstanceAssign( UIContext& context, Container& container )
{
	for( ContainerID& childContainerID : mChildContainerIDs )
	{
		childContainerID = CreateChildContainer(
			context.GetMutableContainerManager(),
			container,
			container.mLeftConstraint,
			container.mRightConstraint,
			container.mTopConstraint,
			container.mBottomConstraint );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
