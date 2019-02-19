#include "stdafx.h"
#include "TextLabel.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::TextLabel )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::TextLabel );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

void TextLabel::SetFont( FontPurposeID purpose )
{
	mFontPurposeID = purpose;

	// Clear these to cause them to load later
	mFontID = gfx::kInvalidManagedFontID;
	mDesiredHeight = 0;
}



void TextLabel::SetText( char const* text )
{
	mText = text;
}



void TextLabel::SetColor( math::Color3f color )
{
	mColor = color;
}



void TextLabel::OnRender( ContainerManager const& manager, Container const& container, bool& blockChildRendering )
{
	gfx::PPUController& renderer = GetRenderer( manager );

	if( mDesiredHeight == 0 || mFontID == gfx::kInvalidManagedFontID )
	{
		Font const font = GetFontRegistry( manager ).SelectBestFont( mFontPurposeID, renderer.GetCurrentZoomFactor() );
		mFontID = font.mManagedFontID;
		mDesiredHeight = font.mFontHeight;
	}

	renderer.DrawText( container.mAABB.mTopLeft, manager.GetRecommendedRenderDepth( container ), mDesiredHeight, mFontID, mColor, "%s", mText.c_str() );
}



void TextLabel::OnAABBRecalc( ContainerManager& manager, Container& container )
{
	// Clear these in case there's a better font now
	mFontID = gfx::kInvalidManagedFontID;
	mDesiredHeight = 0;
}

///////////////////////////////////////////////////////////////////////////////
}}}
