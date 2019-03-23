#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Passthrough final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( Passthrough );

	//
	// Public methods
public:
	Passthrough() = default;

	ContainerID GetChildContainerID() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	ContainerID mContainerID = kInvalidContainerID;
};

///////////////////////////////////////////////////////////////////////////////
}}}
