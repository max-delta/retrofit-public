#include "stdafx.h"
#include "TextLabel.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::TextLabel )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::TextLabel );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

void TextLabel::SetFont( gfx::ManagedFontID fontID, uint8_t desiredHeight )
{
	mFontID = fontID;
	mDesiredHeight = desiredHeight;
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
	renderer.DrawText( container.mAABB.mTopLeft, manager.GetRecommendedRenderDepth( container ), mDesiredHeight, mFontID, mColor, "%s", mText.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
}}}
