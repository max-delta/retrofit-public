#include "stdafx.h"
#include "Floater.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/Lerp.h"


RFTYPE_CREATE_META( RF::ui::controller::Floater )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::Floater );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

Floater::Floater( gfx::PPUCoordElem width, gfx::PPUCoordElem height, Justification justification )
	: mJustification( justification )
	, mDimensions( width, height )
{
	//
}



ContainerID Floater::GetChildContainerID() const
{
	return mContainer;
}



void Floater::OnAssign( ContainerManager& manager, Container& container )
{
	ContainerID const parentContainerID = container.mContainerID;

	mTopLeftAnchor = CreateAnchor( manager, container );
	mBottomRightAnchor = CreateAnchor( manager, container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mContainer = Controller::CreateChildContainer( manager, container, left, right, top, bottom );
}



void Floater::OnAABBRecalc( ContainerManager& manager, Container& container )
{
	gfx::PPUCoord pos = container.mAABB.mTopLeft;
	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Top ) )
	{
		pos.y = container.mAABB.Top();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Bottom ) )
	{
		pos.y = container.mAABB.Bottom() - mDimensions.y;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Middle ) )
	{
		gfx::PPUCoordElem const top = container.mAABB.Top();
		gfx::PPUCoordElem const bottom = container.mAABB.Bottom() - mDimensions.y;
		pos.y = math::Lerp( top, bottom, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Left ) )
	{
		pos.x = container.mAABB.Left();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Right ) )
	{
		pos.x = container.mAABB.Right() - mDimensions.x;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Center ) )
	{
		gfx::PPUCoordElem const left = container.mAABB.Left();
		gfx::PPUCoordElem const right = container.mAABB.Right() - mDimensions.x;
		pos.x = math::Lerp( left, right, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	MoveAnchor( manager, mTopLeftAnchor, pos );
	MoveAnchor( manager, mBottomRightAnchor, pos + mDimensions );
}

///////////////////////////////////////////////////////////////////////////////
}}}
