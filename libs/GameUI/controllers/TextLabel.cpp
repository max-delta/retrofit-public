#include "stdafx.h"
#include "TextLabel.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/rf_onceper.h"


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

	// Clear to cause font to re-load later
	InvalidateFont();
}



void TextLabel::SetText( rftl::string const& text )
{
	mText = text;
}



void TextLabel::SetText( rftl::string&& text )
{
	mText = rftl::move( text );
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



void TextLabel::SetIgnoreOverflow( bool ignoreOverflow )
{
	mIgnoreOverflow = ignoreOverflow;
}



bool TextLabel::HasText() const
{
	return mText.empty() == false;
}



void TextLabel::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	if( HasText() == false )
	{
		return;
	}

	gfx::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	if( mDesiredHeight == 0 || mFontID == gfx::kInvalidManagedFontID )
	{
		Font const font = GetFontRegistry( context.GetContainerManager() ).SelectBestFont( mFontPurposeID, renderer.GetCurrentZoomFactor() );
		mFontID = font.mManagedFontID;
		mDesiredHeight = font.mFontHeight;
		mBaselineOffset = font.mBaselineOffset;
	}
	RF_ASSERT( mFontID != gfx::kInvalidManagedFontID );
	RF_ASSERT( mDesiredHeight > mBaselineOffset );

	gfx::PPUCoordElem const stringWidth = renderer.CalculateStringLength( mDesiredHeight, mFontID, mText.c_str() );
	gfx::PPUCoord const expectedDimensions = {
		stringWidth,
		// Push the descenders down below the baseline
		//  (tails like g, j, p, q, y )
		math::integer_cast<gfx::PPUCoordElem>( mDesiredHeight - mBaselineOffset )
	};
	gfx::PPUCoord const pos = AlignToJustify( expectedDimensions, container.mAABB, mJustification );

#if RF_IS_ALLOWED( RF_CONFIG_ONCEPER )
	if( mIgnoreOverflow == false )
	{
		if( container.mAABB.Height() < mDesiredHeight )
		{
			RF_ONCEPER_SECOND( RFLOG_WARNING( nullptr, RFCAT_GAMEUI, "A label cannot fit into its AABB height: '%s'", mText.c_str() ) );
		}
		if( container.mAABB.Width() < renderer.CalculateStringLength( mDesiredHeight, mFontID, mText.c_str() ) )
		{
			RF_ONCEPER_SECOND( RFLOG_WARNING( nullptr, RFCAT_GAMEUI, "A label cannot fit into its AABB width: '%s'", mText.c_str() ) );
		}
	}
#endif

	renderer.DrawText( pos, context.GetContainerManager().GetRecommendedRenderDepth( container ), mDesiredHeight, mFontID, mBorder, mColor, "%s", mText.c_str() );
}



void TextLabel::OnAABBRecalc( UIContext& context, Container& container )
{
	// Clear in case there's a better font now
	InvalidateFont();
}



void TextLabel::OnZoomFactorChange( UIContext& context, Container& container )
{
	// Clear in case there's a better font now
	InvalidateFont();
}



void TextLabel::InvalidateFont()
{
	// Clear these in case there's a better font now
	mFontID = gfx::kInvalidManagedFontID;
	mDesiredHeight = 0;
}

///////////////////////////////////////////////////////////////////////////////
}}}
