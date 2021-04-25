#include "stdafx.h"
#include "PaddedPassthrough.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::PaddedPassthrough )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::PaddedPassthrough );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

PaddedPassthrough::PaddedPassthrough( gfx::ppu::PPUCoord paddingDimensions )
	: mPaddingDimensions( rftl::move( paddingDimensions ) )
{
	//
}



ContainerID PaddedPassthrough::GetChildContainerID() const
{
	return mChildContainerID;
}



void PaddedPassthrough::OnInstanceAssign( UIContext& context, Container& container )
{
	mTopLeftAnchor = CreateAnchor( context.GetMutableContainerManager(), container );
	mBottomRightAnchor = CreateAnchor( context.GetMutableContainerManager(), container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mChildContainerID = Controller::CreateChildContainer( context.GetMutableContainerManager(), container, left, right, top, bottom );
}



void PaddedPassthrough::OnAABBRecalc( UIContext& context, Container& container )
{
	gfx::ppu::PPUCoord const topLeft = container.mAABB.mTopLeft + mPaddingDimensions;
	gfx::ppu::PPUCoord const bottomRight = container.mAABB.mBottomRight - mPaddingDimensions;
	MoveAnchor( context.GetMutableContainerManager(), mTopLeftAnchor, topLeft );
	MoveAnchor( context.GetMutableContainerManager(), mBottomRightAnchor, bottomRight );
}

///////////////////////////////////////////////////////////////////////////////
}
