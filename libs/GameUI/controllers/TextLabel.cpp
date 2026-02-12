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

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

void TextLabel::SetFont( FontPurposeID purpose )
{
	RF_ASSERT( purpose != kInvalidFontPurposeID );
	mFontPurposeID = purpose;

	// Clear to cause font to re-load later
	InvalidateFont();
}



void TextLabel::SetText( rftl::string_view text )
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



void TextLabel::SetJustification( Justification::Value justification )
{
	mJustification = justification;
}



void TextLabel::SetColor( math::Color3u8 color )
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

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	if( mDesiredHeight == 0 || mFontID == gfx::kInvalidManagedFontID )
	{
		RF_ASSERT( mFontPurposeID != kInvalidFontPurposeID );
		Font const font = GetFontRegistry( context.GetContainerManager() ).SelectBestFont( mFontPurposeID, renderer.GetCurrentZoomFactor() );
		mFontID = font.mManagedFontID;
		mDesiredHeight = font.mFontHeight;
		mBaselineOffset = font.mBaselineOffset;
	}
	RF_ASSERT( mFontID != gfx::kInvalidManagedFontID );
	RF_ASSERT( mDesiredHeight > mBaselineOffset );

	gfx::ppu::Vec2 const expectedDimensions = CalculatePrimaryFontExtents( renderer, mFontID, mDesiredHeight, mBaselineOffset, mText );
	gfx::ppu::Coord const pos = AlignToJustify( expectedDimensions, container.mAABB, mJustification );

	if constexpr( config::flag::kOncePer )
	{
		if( mIgnoreOverflow == false )
		{
			if( container.mAABB.Height() < mDesiredHeight )
			{
				RF_ONCEPER_SECOND( RFLOGF_WARNING( nullptr, RFCAT_GAMEUI, "A label cannot fit into its AABB height: '{}'", mText ) );
			}
			if( container.mAABB.Width() < renderer.CalculateStringLength( mDesiredHeight, mFontID, mText ) )
			{
				RF_ONCEPER_SECOND( RFLOGF_WARNING( nullptr, RFCAT_GAMEUI, "A label cannot fit into its AABB width: '{}'", mText ) );
			}
		}
	}

	renderer.DrawText( pos, context.GetContainerManager().GetRecommendedRenderDepth( container ), mDesiredHeight, mFontID, mBorder, mColor, "{}", mText );
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
}
