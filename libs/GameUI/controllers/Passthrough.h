#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Performs essentially a no-op, but can still have operations applied to it,
//  notably being part of the focus tree and the render tree, so it can be used
//  as a convenient way to generically shunt around large parts of UI logic
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
	ContainerID mChildContainerID = kInvalidContainerID;
};

///////////////////////////////////////////////////////////////////////////////
}
