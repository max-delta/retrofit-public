#include "stdafx.h"
#include "Controller.h"

#include "GameUI/ContainerManager.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::Controller )
{
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::Controller );
}

namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

void Controller::OnAssign( UIContext& context, Container& container )
{
	//
}



void Controller::OnAABBRecalc( UIContext& context, Container& container )
{
	//
}



void Controller::OnImminentDestruction( UIContext& context, Container const& container )
{
	//
}



void Controller::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	//
}



gfx::PPUController& Controller::GetRenderer( ContainerManager const& manager ) const
{
	return manager.GetRenderer();
}



FontRegistry const& Controller::GetFontRegistry( ContainerManager const& manager ) const
{
	return manager.GetFontRegistry();
}



Container& Controller::GetMutableContainer( ContainerManager& manager, ContainerID containerID )
{
	return manager.GetMutableContainer( containerID );
}



ContainerID Controller::CreateChildContainer(
	ContainerManager& manager,
	Container& parentContainer,
	AnchorID leftConstraint,
	AnchorID rightConstraint,
	AnchorID topConstraint,
	AnchorID bottomConstraint )
{
	return manager.CreateChildContainer(
		parentContainer,
		leftConstraint,
		rightConstraint,
		topConstraint,
		bottomConstraint );
}



void Controller::DestroyContainer( ContainerManager& manager, ContainerID containerID )
{
	manager.DestroyContainer( containerID );
}



AnchorID Controller::CreateAnchor( ContainerManager& manager, Container& container )
{
	return manager.CreateAnchor( container );
}



void Controller::MoveAnchor( ContainerManager& manager, AnchorID anchorID, gfx::PPUCoord pos )
{
	manager.MoveAnchor( anchorID, pos );
}

///////////////////////////////////////////////////////////////////////////////
}}
