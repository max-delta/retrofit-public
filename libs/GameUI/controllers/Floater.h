#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Floater final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( Floater );

	//
	// Public methods
public:
	Floater() = delete;
	Floater( gfx::PPUCoordElem width, gfx::PPUCoordElem height, Justification justification );

	ContainerID GetChildContainerID() const;

	gfx::PPUCoord GetOffset() const;
	void SetOffset( UIContext& context, gfx::PPUCoord offset );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	AnchorID mTopLeftAnchor = kInvalidAnchorID;
	AnchorID mBottomRightAnchor = kInvalidAnchorID;
	ContainerID mContainer = kInvalidContainerID;
	Justification mJustification = Justification::TopLeft;
	gfx::PPUCoord mDimensions = {};
	gfx::PPUCoord mOffset = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}
