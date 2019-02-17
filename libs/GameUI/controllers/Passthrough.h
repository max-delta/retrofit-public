#pragma once
#include "project.h"

#include "GameUI/Controller.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Passthrough final : public Controller
{
	//
	// Public methods
public:
	ContainerID GetChildContainerID();

	virtual void OnAssign( ContainerManager& manager, Container& container ) override;


	//
	// Private data
private:
	ContainerID mContainerID = kInvalidContainerID;
};

///////////////////////////////////////////////////////////////////////////////
}}}
