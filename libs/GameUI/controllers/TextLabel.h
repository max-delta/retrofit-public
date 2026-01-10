#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Color3f.h"

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
	void SetColor( math::Color3f color );
	void SetBorder( bool border );
	void SetIgnoreOverflow( bool ignoreOverflow );

	bool HasText() const;

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
	uint8_t mDesiredHeight = 0;
	uint8_t mBaselineOffset = 0;
	Justification::Value mJustification = Justification::TopLeft;
	rftl::string mText;
	math::Color3f mColor = math::Color3f::kBlack;
	bool mBorder = false;
	bool mIgnoreOverflow = false;
};

///////////////////////////////////////////////////////////////////////////////
}
