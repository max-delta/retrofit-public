#include "stdafx.h"
#include "TextLabel.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"

#include "PPU/PPUController.h"


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



void TextLabel::OnRender( ContainerManager const& manager, Container const& container, bool& blockChildRendering )
{
	gfx::PPUController& renderer = GetRenderer( manager );
	renderer.DrawText( container.mAABB.mTopLeft, manager.GetRecommendedRenderDepth( container ), mDesiredHeight, mFontID, "%s", mText.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
}}}
