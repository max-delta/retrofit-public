#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Color3u8.h"
#include "core_localization/LineBreakRules.h"

#include "rftl/extension/cstring_view.h"


// Forwards
namespace RF::ui::controller {
class TextRows;
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Text box that performs line-wrapping and truncation
class GAMEUI_API TextBox : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( TextBox );

	//
	// Public methods
public:
	TextBox() = delete;
	TextBox(
		size_t numRows,
		FontPurposeID purpose,
		loc::LineBreakRules const& lineBreakRules );
	TextBox(
		size_t numRows,
		FontPurposeID purpose,
		Justification::Value justification,
		math::Color3u8 color,
		loc::LineBreakRules const& lineBreakRules );

	ContainerID GetChildContainerID() const;

	// NOTE: Right-to-left text is parsed right-to-left, so the leftmost text
	//  is the text that will get truncated first from the string argument
	void SetText( rftl::string_view text, bool rightToLeft );
	bool IsRightToLeft() const;

	// Intended for use in evaluating whether text will fit or not, in advance
	void SpeculativelySplitAcrossLines(
		UIConstContext const& context,
		Container const& container,
		rftl::vector<rftl::string_view>& textLines,
		rftl::string_view& truncated,
		rftl::string_view str ) const;

	rftl::vector<gfx::ppu::AABB> GetTextAABBsUsedLastRender() const;
	size_t GetNumCharactersUnwrittenLastRender() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	size_t const mNumRows;
	FontPurposeID const mFontPurpose;
	Justification::Value const mJustification;
	bool mRightToLeft = false;
	math::Color3u8 const mColor;
	loc::LineBreakRules const mLineBreakRules;

	ContainerID mChildContainerID = kInvalidContainerID;
	WeakPtr<TextRows> mSlotController;

	rftl::string mText;

	size_t mNumCharactersUnwrittenLastRender = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
