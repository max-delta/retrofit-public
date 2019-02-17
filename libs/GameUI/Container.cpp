#include "stdafx.h"
#include "Container.h"

#include "GameUI/Controller.h"


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
	Controller* const controller = mWeakUIController;
	RF_ASSERT( controller != nullptr );
	controller->OnAssign( manager, *this );
	controller->OnAABBRecalc( manager, *this );
}



void Container::OnAABBRecalc( ContainerManager& manager )
{
	Controller* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnAABBRecalc( manager, *this );
	}
}



void Container::OnImminentDestruction( ContainerManager& manager ) const
{
	Controller* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnImminentDestruction( manager, *this );
	}
}



void Container::OnRender( ContainerManager const& manager, bool& blockChildRendering ) const
{
	Controller* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnRender( manager, *this, blockChildRendering );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
