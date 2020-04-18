#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API PaddedPassthrough final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( PaddedPassthrough );

	//
	// Public methods
public:
	PaddedPassthrough() = delete;
	PaddedPassthrough( gfx::PPUCoord paddingDimensions );

	ContainerID GetChildContainerID() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	AnchorID mTopLeftAnchor = kInvalidAnchorID;
	AnchorID mBottomRightAnchor = kInvalidAnchorID;
	ContainerID mChildContainerID = kInvalidContainerID;
	gfx::PPUCoord mPaddingDimensions = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}
