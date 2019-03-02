#pragma once
#include "project.h"

#include "GameUI/Controller.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Passthrough final : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	ContainerID GetChildContainerID();

	virtual void OnAssign( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	ContainerID mContainerID = kInvalidContainerID;
};

///////////////////////////////////////////////////////////////////////////////
}}}
