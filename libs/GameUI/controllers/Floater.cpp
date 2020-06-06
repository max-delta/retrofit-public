#include "stdafx.h"
#include "Floater.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::Floater )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
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



void Floater::OnInstanceAssign( UIContext& context, Container& container )
{
	mTopLeftAnchor = CreateAnchor( context.GetMutableContainerManager(), container );
	mBottomRightAnchor = CreateAnchor( context.GetMutableContainerManager(), container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mContainer = Controller::CreateChildContainer( context.GetMutableContainerManager(), container, left, right, top, bottom );
}



void Floater::OnAABBRecalc( UIContext& context, Container& container )
{
	gfx::PPUCoord const pos = AlignToJustify( mDimensions, container.mAABB, mJustification );

	MoveAnchor( context.GetMutableContainerManager(), mTopLeftAnchor, pos );
	MoveAnchor( context.GetMutableContainerManager(), mBottomRightAnchor, pos + mDimensions );
}

///////////////////////////////////////////////////////////////////////////////
}}}
