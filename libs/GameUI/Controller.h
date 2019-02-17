#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Controller
{
	//
	// Public methods
public:
	virtual ~Controller() = default;

	virtual void OnAssign( ContainerManager& manager, Container& container );
	virtual void OnAABBRecalc( ContainerManager& manager, Container& container );

	// NOTE: Do NOT destroy child containers or anchors, this will happen
	//  automatically, and attempting to do so here may cause re-entrancy
	//  violations and undefined behavior
	// NOTE: Container is const to help enforce this
	virtual void OnImminentDestruction( ContainerManager& manager, Container const& container );


	//
	// Protected methods
protected:
	Container& GetMutableContainer( ContainerManager& manager, ContainerID containerID );
	ContainerID CreateChildContainer(
		ContainerManager& manager,
		Container& parentContainer,
		AnchorID leftConstraint,
		AnchorID rightConstraint,
		AnchorID topConstraint,
		AnchorID bottomConstraint );
	void DestroyContainer( ContainerManager& manager, ContainerID containerID );

	AnchorID CreateAnchor( ContainerManager& manager, Container& container );
	void MoveAnchor( ContainerManager& manager, AnchorID anchorID, gfx::PPUCoord pos );
};

///////////////////////////////////////////////////////////////////////////////
}}
