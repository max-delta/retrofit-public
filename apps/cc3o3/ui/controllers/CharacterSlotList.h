#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/GenericListBox.h"

// Forwards
namespace RF::cc::ui::controller {
class CharacterSlot;
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class CharacterSlotList final : public GenericListBox
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( CharacterSlotList );

	//
	// Public methods
public:
	CharacterSlotList();

	bool HasCharacter( size_t slotIndex ) const;
	void ClearCharacter( size_t slotIndex );
	void UpdateCharacter( size_t slotIndex, state::ObjectRef const& character );

	WeakPtr<CharacterSlot const> GetSlotController( size_t slotIndex ) const;
	WeakPtr<CharacterSlot> GetMutableSlotController( size_t slotIndex );


	//
	// Private methods
private:
	virtual void PostInstanceAssign( UIContext& context, Container& container ) override;
	virtual bool ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const override;
};

///////////////////////////////////////////////////////////////////////////////
}
