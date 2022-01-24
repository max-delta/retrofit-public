#pragma once
#include "project.h"

#include "cc3o3/options/OptionsFwd.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/InstancedController.h"


// Forwards
namespace RF::ui::controller {
class ColumnSlicer;
class TextLabel;
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class OptionSlot final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( OptionSlot );

	//
	// Public methods
public:
	OptionSlot() = default;

	bool HasOption() const;
	void ClearOption();
	void UpdateOption( options::Option const& option );

	void UpdateState( bool active, bool selected );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	ContainerID mChildContainerID = kInvalidContainerID;
	WeakPtr<ColumnSlicer> mColumnSlicer;
	WeakPtr<TextLabel> mNameLabel;
	WeakPtr<TextLabel> mValueLabel;
};

///////////////////////////////////////////////////////////////////////////////
}
