#pragma once
#include "project.h"

#include "GameUI/Controller.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Floater final : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	Floater() = delete;
	Floater( gfx::PPUCoordElem width, gfx::PPUCoordElem height, Justification justification );

	ContainerID GetChildContainerID() const;

	virtual void OnAssign( ContainerManager& manager, Container& container ) override;
	virtual void OnAABBRecalc( ContainerManager& manager, Container& container ) override;


	//
	// Private data
private:
	AnchorID mTopLeftAnchor = kInvalidAnchorID;
	AnchorID mBottomRightAnchor = kInvalidAnchorID;
	ContainerID mContainer = kInvalidContainerID;
	Justification mJustification = Justification::TopLeft;
	gfx::PPUCoord mDimensions = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}
