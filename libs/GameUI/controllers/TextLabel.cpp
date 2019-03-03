#include "stdafx.h"
#include "TextLabel.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/Lerp.h"


RFTYPE_CREATE_META( RF::ui::controller::TextLabel )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
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



void TextLabel::SetJustification( Justification justification )
{
	mJustification = justification;
}



void TextLabel::SetColor( math::Color3f color )
{
	mColor = color;
}



void TextLabel::SetBorder( bool border )
{
	mBorder = border;
}



void TextLabel::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	gfx::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	if( mDesiredHeight == 0 || mFontID == gfx::kInvalidManagedFontID )
	{
		Font const font = GetFontRegistry( context.GetContainerManager() ).SelectBestFont( mFontPurposeID, renderer.GetCurrentZoomFactor() );
		mFontID = font.mManagedFontID;
		mDesiredHeight = font.mFontHeight;
	}

	gfx::PPUCoord pos = container.mAABB.mTopLeft;
	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Top ) )
	{
		pos.y = container.mAABB.Top();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Bottom ) )
	{
		pos.y = container.mAABB.Bottom() - mDesiredHeight;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Middle ) )
	{
		gfx::PPUCoordElem const top = container.mAABB.Top();
		gfx::PPUCoordElem const bottom = container.mAABB.Bottom() - mDesiredHeight;
		pos.y = math::Lerp( top, bottom, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Left ) )
	{
		pos.x = container.mAABB.Left();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Right ) )
	{
		gfx::PPUCoordElem const stringWidth = renderer.CalculateStringLength( mDesiredHeight, mFontID, mText.c_str() );
		pos.x = container.mAABB.Right() - stringWidth;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Center ) )
	{
		gfx::PPUCoordElem const stringWidth = renderer.CalculateStringLength( mDesiredHeight, mFontID, mText.c_str() );
		gfx::PPUCoordElem const left = container.mAABB.Left();
		gfx::PPUCoordElem const right = container.mAABB.Right() - stringWidth;
		pos.x = math::Lerp( left, right, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	renderer.DrawText( pos, context.GetContainerManager().GetRecommendedRenderDepth( container ), mDesiredHeight, mFontID, mBorder, mColor, "%s", mText.c_str() );
}



void TextLabel::OnAABBRecalc( UIContext& context, Container& container )
{
	// Clear these in case there's a better font now
	mFontID = gfx::kInvalidManagedFontID;
	mDesiredHeight = 0;
}

///////////////////////////////////////////////////////////////////////////////
}}}
