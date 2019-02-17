#include "stdafx.h"
#include "Controller.h"

#include "GameUI/ContainerManager.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

void Controller::OnAssign( ContainerManager& manager, Container& container )
{
	//
}



void Controller::OnAABBRecalc( ContainerManager& manager, Container& container )
{
	//
}



void Controller::OnImminentDestruction( ContainerManager& manager, Container const& container )
{
	//
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
