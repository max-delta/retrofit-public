#include "stdafx.h"
#include "FontRegistry.h"

#include "rftl/algorithm"
#include "rftl/functional"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

void FontRegistry::RegisterFallbackFont( Font const& font )
{
	mFallbackFont = font;
}



void FontRegistry::RegisterFont( FontPurposeID purpose, Font const& font )
{
	RF_ASSERT( purpose != kInvalidFontPurposeID );
	RF_ASSERT( font.mManagedFontID != gfx::kInvalidManagedFontID );
	RF_ASSERT( font.mFontHeight > 0 );
	RF_ASSERT( font.mMinimumZoomFactor != gfx::ppu::kInvalidZoomFactor );

	FontsByZoomFactor& fontsByZoomFactor = mFontsByPurpose[purpose];

	fontsByZoomFactor.emplace_back( font );

	rftl::sort(
		fontsByZoomFactor.begin(),
		fontsByZoomFactor.end(),
		[]( Font const& lhs, Font const& rhs ) -> bool
		{
			// Reverse order
			return lhs.mMinimumZoomFactor > rhs.mMinimumZoomFactor;
		} );
}



Font FontRegistry::SelectBestFont( FontPurposeID purpose, gfx::ppu::ZoomFactor zoomFactor ) const
{
	FontsByPurpose::const_iterator const purposeIter = mFontsByPurpose.find( purpose );
	if( purposeIter == mFontsByPurpose.end() )
	{
		// No font for this purpose
		return mFallbackFont;
	}
	FontsByZoomFactor const& fontsByZoomFactor = purposeIter->second;

	for( Font const& font : fontsByZoomFactor )
	{
		if( zoomFactor < font.mMinimumZoomFactor )
		{
			// Font too high quality for this zoom factor
			continue;
		}

		// Highest quality font for this zoom factor
		return font;
	}

	// No font for this zoom factor
	return mFallbackFont;
}

///////////////////////////////////////////////////////////////////////////////
}
