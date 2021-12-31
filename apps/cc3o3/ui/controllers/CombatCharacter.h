#pragma once
#include "project.h"

#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/InstancedController.h"

#include "rftl/array"

// Forwards
namespace RF::ui::controller {
class BorderFrame;
class TextLabel;
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class CombatCharacter final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( CombatCharacter );

	//
	// Public methods
public:
	CombatCharacter() = default;

	bool HasCharacter() const;
	void ClearCharacter();
	void UpdateCharacter( combat::Fighter const& fighter, state::ObjectRef const& character, bool selected );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	ContainerID mChildContainerID = kInvalidContainerID;
	WeakPtr<BorderFrame> mBorderFrame;
	rftl::array<WeakPtr<TextLabel>, 4> mInfoRows;
};

///////////////////////////////////////////////////////////////////////////////
}
