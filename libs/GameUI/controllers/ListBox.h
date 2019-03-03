#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Color3f.h"
#include "core/ptr/unique_ptr.h"

// Forwards
namespace RF { namespace ui { namespace controller {
	class TextLabel;
}}}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API ListBox final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ListBox );

	//
	// Public methods
public:
	ListBox() = delete;
	ListBox( size_t numSlots, FontPurposeID purpose );
	ListBox(
		size_t numSlots,
		FontPurposeID purpose,
		Justification justification,
		math::Color3f unfocusedColor,
		math::Color3f unselectedColor,
		math::Color3f selectedColor );

	ContainerID GetChildContainerID() const;
	WeakPtr<TextLabel> GetSlotController( size_t slotIndex );
	void SetText( rftl::vector<rftl::string> const& text );

	virtual void OnAssign( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	size_t const mNumSlots;
	FontPurposeID const mFontPurpose;
	Justification const mJustification;
	math::Color3f const mUnfocusedColor;
	math::Color3f const mUnselectedColor;
	math::Color3f const mSelectedColor;
	ContainerID mChildContainerID = kInvalidContainerID;
	rftl::vector<WeakPtr<TextLabel>> mSlotControllers;
};

///////////////////////////////////////////////////////////////////////////////
}}}
