#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/state/StateFwd.h"

#include "GameUI/controllers/InstancedController.h"

#include "rftl/array"

// Forwards
namespace RF { namespace ui { namespace controller {
class BorderFrame;
class FramePackDisplay;
class TextLabel;
}}}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class CharacterSlot final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( CharacterSlot );

	//
	// Public methods
public:
	CharacterSlot() = default;

	bool HasCharacter() const;
	void ClearCharacter();
	void UpdateCharacter( state::ObjectRef const& character );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	ContainerID mChildContainerID = kInvalidContainerID;
	WeakPtr<BorderFrame> mBorderFrame;
	WeakPtr<FramePackDisplay> mCharacterDisplay;
	rftl::array<WeakPtr<TextLabel>, 3> mInfoRows;
};

///////////////////////////////////////////////////////////////////////////////
}
