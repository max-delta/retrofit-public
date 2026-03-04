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

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

Floater::Floater( gfx::ppu::CoordElem width, gfx::ppu::CoordElem height, Justification::Value justification )
	: mJustification( justification )
	, mDimensions( width, height )
{
	//
}



ContainerID Floater::GetChildContainerID() const
{
	return mContainer;
}



gfx::ppu::Coord Floater::GetOffset() const
{
	return mOffset;
}



void Floater::SetOffset( UIContext& context, gfx::ppu::Coord offset )
{
	mOffset = offset;
	RequestHardRecalc( context );
}



void Floater::OnInstanceAssign( UIContext& context, Container& container )
{
	ContainerManager& uiManager = context.GetMutableContainerManager();

	mTopLeftAnchor = CreateAnchor( uiManager, container );
	mBottomRightAnchor = CreateAnchor( uiManager, container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mContainer = Controller::CreateChildContainer( uiManager, container, left, right, top, bottom );
}



void Floater::OnAABBRecalc( UIContext& context, Container& container )
{
	gfx::ppu::Coord const pos = AlignToJustify( mDimensions, container.mAABB, mJustification ) + mOffset;

	MoveAnchor( context.GetMutableContainerManager(), mTopLeftAnchor, pos );
	MoveAnchor( context.GetMutableContainerManager(), mBottomRightAnchor, pos + mDimensions );
}

///////////////////////////////////////////////////////////////////////////////
}
