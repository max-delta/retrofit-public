#pragma once
#include "project.h"

#include "GameUI/Controller.h"

#include "core_math/Color3f.h"

#include "rftl/string"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API TextLabel final : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	void SetFont( FontPurposeID purpose );
	void SetText( char const* text );
	void SetJustification( Justification justification );
	void SetColor( math::Color3f color );
	void SetBorder( bool border );

	virtual void OnRender( ContainerManager const& manager, Container const& container, bool& blockChildRendering ) override;
	virtual void OnAABBRecalc( ContainerManager& manager, Container& container ) override;


	//
	// Private data
private:
	FontPurposeID mFontPurposeID = kInvalidFontPurposeID;
	gfx::ManagedFontID mFontID = gfx::kInvalidManagedFontID;
	uint8_t mDesiredHeight = 0;
	Justification mJustification = Justification::TopLeft;
	rftl::string mText;
	math::Color3f mColor = math::Color3f::kBlack;
	bool mBorder = false;
};

///////////////////////////////////////////////////////////////////////////////
}}}
