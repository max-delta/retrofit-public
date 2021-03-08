#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API MultiPassthrough final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( MultiPassthrough );

	//
	// Public methods
public:
	MultiPassthrough( size_t numChildren );

	ContainerID GetChildContainerID( size_t index ) const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	rftl::vector<ContainerID> mChildContainerIDs;
};

///////////////////////////////////////////////////////////////////////////////
}
