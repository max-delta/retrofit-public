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

PaddedPassthrough::PaddedPassthrough( gfx::ppu::Coord paddingDimensions )
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
	ContainerManager& uiManager = context.GetMutableContainerManager();

	mTopLeftAnchor = CreateAnchor( uiManager, container );
	mBottomRightAnchor = CreateAnchor( uiManager, container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mChildContainerID = Controller::CreateChildContainer( uiManager, container, left, right, top, bottom );
}



void PaddedPassthrough::OnAABBRecalc( UIContext& context, Container& container )
{
	gfx::ppu::Coord const topLeft = container.mAABB.mTopLeft + mPaddingDimensions;
	gfx::ppu::Coord const bottomRight = container.mAABB.mBottomRight - mPaddingDimensions;
	MoveAnchor( context.GetMutableContainerManager(), mTopLeftAnchor, topLeft );
	MoveAnchor( context.GetMutableContainerManager(), mBottomRightAnchor, bottomRight );
}

///////////////////////////////////////////////////////////////////////////////
}
