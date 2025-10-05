#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Color3f.h"

#include "rftl/extension/cstring_view.h"


// Forwards
namespace RF::ui::controller {
	class TextRows;
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API TextBox final : public InstancedController
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
		rftl::unordered_set<char> const& breakableChars );
	TextBox(
		size_t numRows,
		FontPurposeID purpose,
		Justification::Value justification,
		math::Color3f color,
		rftl::unordered_set<char> const& breakableChars );

	ContainerID GetChildContainerID() const;

	// NOTE: Right-to-left text is parsed right-to-left, so the leftmost text
	//  is the text that will get truncated first from the string argument
	void SetText( rftl::string_view text, bool rightToLeft );

	size_t GetNumCharactersUnwrittenLastRender() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private data
private:
	size_t const mNumRows;
	FontPurposeID const mFontPurpose;
	Justification::Value const mJustification;
	math::Color3f const mColor;
	ContainerID mChildContainerID = kInvalidContainerID;
	bool mRightToLeft = false;
	rftl::string mText;
	rftl::unordered_set<char> mBreakableChars;
	WeakPtr<TextRows> mSlotController;

	size_t mNumCharactersUnwrittenLastRender = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
