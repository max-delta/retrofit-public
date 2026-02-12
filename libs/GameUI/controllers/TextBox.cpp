#include "stdafx.h"
#include "TextBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/TextRows.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"
#include "core/rf_onceper.h"

#include "rftl/extension/algorithms.h"
#include "rftl/extension/string_parse.h"
#include "rftl/algorithm"


RFTYPE_CREATE_META( RF::ui::controller::TextBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::TextBox );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

TextBox::TextBox(
	size_t numRows,
	FontPurposeID purpose,
	rftl::unordered_set<char> const& breakableChars )
	: // Clang-format is trash garbage
	TextBox(
		numRows,
		purpose,
		Justification::MiddleCenter,
		math::Color3u8::kWhite,
		breakableChars )
{
	//
}



TextBox::TextBox(
	size_t numRows,
	FontPurposeID purpose,
	Justification::Value justification,
	math::Color3u8 color,
	rftl::unordered_set<char> const& breakableChars )
	: mNumRows( numRows )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mColor( color )
	, mBreakableChars( breakableChars.begin(), breakableChars.end() )
{
	RF_ASSERT( mNumRows >= 2 );
}



ContainerID TextBox::GetChildContainerID() const
{
	return mChildContainerID;
}



void TextBox::SetText( rftl::string_view text, bool rightToLeft )
{
	mRightToLeft = rightToLeft;
	mText = text;
}



bool TextBox::IsRightToLeft() const
{
	return mRightToLeft;
}



void TextBox::SpeculativelySplitAcrossLines(
	UIConstContext const& context,
	Container const& container,
	rftl::vector<rftl::string_view>& textLines,
	rftl::string_view& truncated,
	rftl::string_view str ) const
{
	textLines.clear();
	truncated = {};

	if( str.empty() )
	{
		return;
	}

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	Font const font = GetFontRegistry( context.GetContainerManager() ).SelectBestFont( mFontPurpose, renderer.GetCurrentZoomFactor() );

	gfx::ppu::CoordElem const maxLineLen = container.mAABB.Width();
	textLines.reserve( mNumRows );

	rftl::string_view const breakableChars = mBreakableChars;
	auto const rewindPastNonBreakableCharacters = [&breakableChars]( rftl::string_view full ) -> rftl::string_view
	{
		RF_ASSERT( full.empty() == false );
		if( breakableChars.empty() )
		{
			return full;
		}

		rftl::string_view partial = full;
		while( partial.empty() == false && breakableChars.find( partial.back() ) == rftl::string_view::npos )
		{
			partial.remove_suffix( 1 );
		}
		return partial;
	};

	auto const rewindPastBreakableCharacters = [&breakableChars]( rftl::string_view full ) -> rftl::string_view
	{
		RF_ASSERT( full.empty() == false );
		if( breakableChars.empty() )
		{
			return full;
		}

		return rftl::trim_suffix_chars( full, breakableChars );
	};

	auto const determineLengthThatFits = [&renderer, &font]( rftl::string_view full, gfx::ppu::CoordElem width ) -> size_t
	{
		RF_ASSERT( full.empty() == false );
		RF_ASSERT( width > 0 );

		rftl::string_view::const_iterator const begin = full.begin();
		rftl::string_view::const_iterator const end = full.end();
		auto const doesFit = [&renderer, &font, &width, &begin]( rftl::string_view::const_iterator iter ) -> bool
		{
			rftl::string_view const partial( begin, iter + 1 );
			return renderer.CalculateStringLength( font.mFontHeight, font.mManagedFontID, partial ) <= width;
		};
		rftl::string_view::const_iterator const onePastTheMaxLen = rftl::partition_point_iter( begin, end, doesFit );
		RF_ASSERT( onePastTheMaxLen > begin );
		RF_ASSERT( onePastTheMaxLen <= end );
		size_t const lenThatFits = math::integer_cast<size_t>( rftl::distance( begin, onePastTheMaxLen ) );
		RF_ASSERT( renderer.CalculateStringLength( font.mFontHeight, font.mManagedFontID, rftl::string_view( begin, onePastTheMaxLen ) ) <= width );
		if( onePastTheMaxLen < end )
		{
			RF_ASSERT( renderer.CalculateStringLength( font.mFontHeight, font.mManagedFontID, rftl::string_view( begin, onePastTheMaxLen + 1 ) ) > width );
		}
		return lenThatFits;
	};

	rftl::string_view unwrittenText = str;
	while( true )
	{
		if( unwrittenText.empty() )
		{
			// Complete
			return;
		}

		if( textLines.size() >= mNumRows )
		{
			// No more lines left
			truncated = unwrittenText;
			return;
		}

		// How much fits in the width?
		rftl::string_view strThatFits;
		{
			size_t const lenThatFits = determineLengthThatFits( unwrittenText, maxLineLen );
			RF_ASSERT( lenThatFits <= unwrittenText.size() );
			strThatFits = unwrittenText.substr( 0, lenThatFits );
		}

		if( strThatFits.size() > gfx::ppu::kMaxStringLen )
		{
			// Hard limit on render draw request
			RF_ONCEPER_SECOND( RFLOGF_WARNING( nullptr, RFCAT_GAMEUI, "A text box has to truncate a line due to a hard limit from the renderer: '{}'", strThatFits ) );
			strThatFits = strThatFits.substr( 0, gfx::ppu::kMaxStringLen );
		}

		if( strThatFits.size() == unwrittenText.size() )
		{
			// Everything fits! Store and bail
			textLines.emplace_back( strThatFits );
			return;
		}

		// Not everything fits, rewind past last breakable sequence
		strThatFits = rewindPastNonBreakableCharacters( strThatFits );
		strThatFits = rewindPastBreakableCharacters( strThatFits );

		// Store and keep going
		textLines.emplace_back( strThatFits );
		unwrittenText.remove_prefix( strThatFits.size() );
		continue;
	}
}



size_t TextBox::GetNumCharactersUnwrittenLastRender() const
{
	return mNumCharactersUnwrittenLastRender;
}



void TextBox::OnInstanceAssign( UIContext& context, Container& container )
{
	mChildContainerID = CreateChildContainer(
		context.GetMutableContainerManager(),
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	WeakPtr<ui::controller::TextRows> const lines =
		context.GetMutableContainerManager().AssignStrongController(
			mChildContainerID,
			DefaultCreator<ui::controller::TextRows>::Create(
				mNumRows,
				mFontPurpose,
				mJustification,
				mColor ) );
	mSlotController = lines;
}



void TextBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	if( mText.empty() )
	{
		mSlotController->SetText( rftl::vector<rftl::string_view>( mNumRows ) );
		mNumCharactersUnwrittenLastRender = 0;
		return;
	}

	rftl::vector<rftl::string_view> textLines;
	rftl::string_view truncated;
	SpeculativelySplitAcrossLines( context, container, textLines, truncated, mText );
	RF_ASSERT( textLines.size() <= mNumRows );
	textLines.resize( mNumRows );

	// Handle right-to-left output
	rftl::vector<rftl::string> tempRTL;
	if( mRightToLeft )
	{
		tempRTL.assign( textLines.begin(), textLines.end() );
		for( rftl::string& line : tempRTL )
		{
			rftl::reverse( line.begin(), line.end() );
		}

		textLines.clear();
		for( rftl::string const& line : tempRTL )
		{
			textLines.emplace_back( line );
		}
	}

	mSlotController->SetText( textLines );
	mNumCharactersUnwrittenLastRender = truncated.size();
}

///////////////////////////////////////////////////////////////////////////////
}
