#pragma once
#include "project.h"

#include "GameUI/UIController.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Passthrough final : public UIController
{
public:
	ContainerID GetChildContainerID();

	virtual void OnAssign( ContainerManager& manager, Container& container ) override;


private:
	ContainerID mContainerID = kInvalidContainerID;
};

///////////////////////////////////////////////////////////////////////////////
}}}
