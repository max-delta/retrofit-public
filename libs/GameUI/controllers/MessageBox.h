#pragma once
#include "project.h"

#include "GameUI/controllers/TextBox.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Text box with features such as animated blitting of text over multiple
//  frames, common in dialogue boxes to convey a sense of a character talking
class GAMEUI_API MessageBox final : public TextBox
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( MessageBox );

	//
	// Public methods
public:
	MessageBox() = delete;
	MessageBox(
		size_t numRows,
		FontPurposeID purpose,
		rftl::unordered_set<char> const& breakableChars );
	MessageBox(
		size_t numRows,
		FontPurposeID purpose,
		Justification::Value justification,
		math::Color3f color,
		rftl::unordered_set<char> const& breakableChars );

	void SetAnimationSpeed( uint8_t charsPerFrame );

	ContainerID GetChildContainerID() const;

	// NOTE: Right-to-left text is parsed right-to-left, so the leftmost text
	//  is the text that will get truncated first from the string argument
	void SetText( rftl::string_view text, bool rightToLeft );

	size_t GetNumCharactersDispatchedLastRender() const;
	size_t GetNumCharactersRenderedLastRender() const;

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;
	virtual void OnZoomFactorChange( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	uint8_t mAnimSpeed = 0;
	rftl::string mFullText;

	size_t mNumCharsDispatched = 0;
	size_t mNumCharsRendered = 0;
	bool mBlockAnimUntilAABBChange = false;
	bool mAABBChanged = false;
};

///////////////////////////////////////////////////////////////////////////////
}
