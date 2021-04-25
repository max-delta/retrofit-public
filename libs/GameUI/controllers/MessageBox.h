#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Color3f.h"

// Forwards
namespace RF::ui::controller {
	class TextBox;
	class BorderFrame;
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API MessageBox final : public InstancedController
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
		Justification justification,
		math::Color3f color,
		rftl::unordered_set<char> const& breakableChars );

	void SetFrameTileset(
		ui::UIContext& context,
		gfx::ManagedTilesetID tileset,
		gfx::ppu::PPUCoord expectedTileDimensions,
		gfx::ppu::PPUCoord expectedPatternDimensions,
		gfx::ppu::PPUCoord paddingDimensions );
	void SetAnimationSpeed( uint8_t charsPerFrame );

	ContainerID GetChildContainerID() const;

	// NOTE: Right-to-left text is parsed right-to-left, so the leftmost text
	//  is the text that will get truncated first from the string argument
	void SetText( rftl::string const& text, bool rightToLeft );

	size_t GetNumCharactersDispatchedLastRender() const;
	size_t GetNumCharactersRenderedLastRender() const;

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;
	virtual void OnZoomFactorChange( UIContext& context, Container& container ) override;


	//
	// Private data
private:
	size_t const mNumRows;
	FontPurposeID const mFontPurpose;
	Justification const mJustification;
	math::Color3f const mColor;
	uint8_t mAnimSpeed = 0;
	ContainerID mChildContainerID = kInvalidContainerID;
	bool mRightToLeft = false;
	rftl::string mText;
	rftl::unordered_set<char> mBreakableChars;
	WeakPtr<BorderFrame> mFrameController;
	WeakPtr<TextBox> mTextController;

	size_t mNumCharsDispatched = 0;
	size_t mNumCharsRendered = 0;
	bool mBlockAnimUntilAABBChange = false;
	bool mAABBChanged = false;
};

///////////////////////////////////////////////////////////////////////////////
}
