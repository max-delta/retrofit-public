#include "stdafx.h"
#include "TextLabel.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/Lerp.h"
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

	gfx::PPUCoord pos = container.mAABB.mTopLeft;
	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Top ) )
	{
		pos.y = container.mAABB.Top() - mBaselineOffset;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Bottom ) )
	{
		pos.y = container.mAABB.Bottom() - mDesiredHeight;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Middle ) )
	{
		gfx::PPUCoordElem const top = container.mAABB.Top() - mBaselineOffset;
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

	#if RF_IS_ALLOWED( RF_CONFIG_ONCEPER )
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

	// Push the descenders down below the baseline (tails like g, j, p, q, y )
	pos.y += mBaselineOffset;

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
