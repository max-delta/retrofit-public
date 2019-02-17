#include "stdafx.h"
#include "Container.h"

#include "GameUI/UIController.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

bool Container::IsConstrainedBy( AnchorID anchorID ) const
{
	if(
		mLeftConstraint == anchorID ||
		mRightConstraint == anchorID ||
		mTopConstraint == anchorID ||
		mBottomConstraint == anchorID )
	{
		return true;
	}
	return false;
}



void Container::OnAssign( ContainerManager& manager )
{
	UIController* const controller = mWeakUIController;
	RF_ASSERT( controller != nullptr );
	controller->OnAssign( manager, *this );
	controller->OnAABBRecalc( manager, *this );
}



void Container::OnAABBRecalc( ContainerManager& manager )
{
	UIController* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnAABBRecalc( manager, *this );
	}
}



void Container::OnImminentDestruction( ContainerManager& manager ) const
{
	UIController* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnImminentDestruction( manager, *this );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
