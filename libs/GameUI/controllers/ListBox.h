#pragma once
#include "project.h"

#include "GameUI/controllers/GenericListBox.h"

#include "core_math/Color3f.h"

// Forwards
namespace RF::ui::controller {
class TextLabel;
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// A horizontal / vertical list of text entries
class GAMEUI_API ListBox final : public GenericListBox
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( ListBox );

	//
	// Public methods
public:
	ListBox() = delete;
	ListBox(
		size_t numSlots,
		FontPurposeID purpose );
	ListBox(
		Orientation orientation,
		size_t numSlots,
		FontPurposeID purpose );
	ListBox(
		Orientation orientation,
		size_t numSlots,
		FontPurposeID purpose,
		Justification::Value justification,
		math::Color3f unfocusedColor,
		math::Color3f unselectedColor,
		math::Color3f selectedColor );

	WeakPtr<TextLabel> GetMutableSlotController( size_t slotIndex );
	void SetText( rftl::vector<rftl::string> const& text );

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private methods
private:
	virtual void PostInstanceAssign( UIContext& context, Container& container ) override;
	virtual bool ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const override;


	//
	// Private data
private:
	FontPurposeID const mFontPurpose;
	Justification::Value const mJustification;
	math::Color3f const mUnfocusedColor;
	math::Color3f const mUnselectedColor;
	math::Color3f const mSelectedColor;
};

///////////////////////////////////////////////////////////////////////////////
}
