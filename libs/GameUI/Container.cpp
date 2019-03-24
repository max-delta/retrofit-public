#include "stdafx.h"
#include "Container.h"

#include "GameUI/Controller.h"
#include "GameUI/UIContext.h"


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



void Container::OnAssign( UIContext& context )
{
	Controller* const controller = mWeakUIController;
	RF_ASSERT( controller != nullptr );
	controller->OnAssign( context, *this );
}



void Container::OnAABBRecalc( UIContext& context )
{
	RF_ASSERT( mAABB.Width() > 0 );
	RF_ASSERT( mAABB.Height() > 0 );

	Controller* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnAABBRecalc( context, *this );
	}
}



void Container::OnZoomFactorChange( UIContext& context )
{
	Controller* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnZoomFactorChange( context, *this );
	}
}



void Container::OnImminentDestruction( UIContext& context ) const
{
	Controller* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnImminentDestruction( context, *this );
	}
}



void Container::OnRender( UIConstContext const& context, bool& blockChildRendering ) const
{
	Controller* const controller = mWeakUIController;
	if( controller != nullptr )
	{
		controller->OnRender( context, *this, blockChildRendering );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
