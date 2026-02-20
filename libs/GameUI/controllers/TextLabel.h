#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/AABB4.h"
#include "core_math/Color3u8.h"

#include "rftl/extension/cstring_view.h"
#include "rftl/string"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// A single line of text
class GAMEUI_API TextLabel final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( TextLabel );

	//
	// Public methods
public:
	TextLabel() = default;

	void SetFont( FontPurposeID purpose );
	void SetText( rftl::string_view text );
	void SetText( rftl::string&& text );
	void SetText( char const* text );
	void SetJustification( Justification::Value justification );
	void SetColor( math::Color3u8 color );
	void SetBorder( bool border );
	void SetIgnoreOverflow( bool ignoreOverflow );

	bool HasText() const;

	gfx::ppu::AABB GetTextAABBUsedLastRender() const;

	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;
	virtual void OnAABBRecalc( UIContext& context, Container& container ) override;
	virtual void OnZoomFactorChange( UIContext& context, Container& container ) override;


	//
	// Private methods
private:
	void InvalidateFont();


	//
	// Private data
private:
	FontPurposeID mFontPurposeID = kInvalidFontPurposeID;
	gfx::ManagedFontID mFontID = gfx::kInvalidManagedFontID;
	Justification::Value mJustification = Justification::TopLeft;
	math::Color3u8 mColor = math::Color3u8::kBlack;
	rftl::string mText;
	uint8_t mDesiredHeight = 0;
	uint8_t mBaselineOffset = 0;
	bool mBorder = false;
	bool mIgnoreOverflow = false;
	gfx::ppu::AABB mLastRenderedTextAABB = {};
};

///////////////////////////////////////////////////////////////////////////////
}
