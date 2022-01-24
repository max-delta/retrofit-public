#pragma once
#include "project.h"

#include "cc3o3/options/OptionsFwd.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/GenericListBox.h"

// Forwards
namespace RF::cc::ui::controller {
class OptionSlot;
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class OptionSlotList final : public GenericListBox
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( OptionSlotList );

	//
	// Public methods
public:
	OptionSlotList() = delete;
	OptionSlotList( size_t numSlots );

	bool HasOption( size_t slotIndex ) const;
	void ClearOption( size_t slotIndex );
	void UpdateOption( size_t slotIndex, options::Option const& option );

	WeakPtr<OptionSlot const> GetSlotController( size_t slotIndex ) const;
	WeakPtr<OptionSlot> GetMutableSlotController( size_t slotIndex );

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private methods
private:
	virtual void PostInstanceAssign( UIContext& context, Container& container ) override;
	virtual bool ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const override;
};

///////////////////////////////////////////////////////////////////////////////
}
