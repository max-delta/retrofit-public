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
	: TextBox(
		numRows,
		purpose,
		Justification::MiddleCenter,
		math::Color3f::kWhite,
		breakableChars )
{
	//
}



TextBox::TextBox(
	size_t numRows,
	FontPurposeID purpose,
	Justification::Value justification,
	math::Color3f color,
	rftl::unordered_set<char> const& breakableChars )
	: mNumRows( numRows )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mColor( color )
	, mBreakableChars( breakableChars )
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
		mSlotController->SetText( rftl::vector<rftl::string>( mNumRows ) );
		mNumCharactersUnwrittenLastRender = 0;
		return;
	}

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	Font const font = GetFontRegistry( context.GetContainerManager() ).SelectBestFont( mFontPurpose, renderer.GetCurrentZoomFactor() );

	// TODO: Optimize
	gfx::ppu::CoordElem const maxLineLen = container.mAABB.Width();
	rftl::vector<rftl::string> textLines;
	textLines.resize( mNumRows );
	rftl::deque<char> unwrittenText( mText.begin(), mText.end() );
	size_t curLine = 0;
	while( unwrittenText.empty() == false && curLine < textLines.size() )
	{
		rftl::string& line = textLines.at( curLine );

		// Tentatively add the next character
		if( mRightToLeft )
		{
			line += unwrittenText.back();
		}
		else
		{
			line += unwrittenText.front();
		}

		// See if it fits
		bool fits = true;
		if( fits && line.length() > gfx::ppu::kMaxStringLen )
		{
			// Hard limit on render draw request
			RF_ONCEPER_SECOND( RFLOGF_WARNING( nullptr, RFCAT_GAMEUI, "A text box has to truncate a line due to a hard limit from the renderer: '{}'", line ) );
			fits = false;
		}
		if( fits && renderer.CalculateStringLength( font.mFontHeight, font.mManagedFontID, line ) > maxLineLen )
		{
			fits = false;
		}

		// Did it fit?
		if( fits == false )
		{
			// Not enough room, undo and move to next line
			line.pop_back();
			curLine++;

			// Recoup any broken sequences back into buffer
			while( line.empty() == false )
			{
				char const ch = line.back();
				bool const isBreakableChar = mBreakableChars.count( ch ) > 0;
				if( isBreakableChar )
				{
					break;
				}
				else
				{
					line.pop_back();
					if( mRightToLeft )
					{
						unwrittenText.push_back( ch );
					}
					else
					{
						unwrittenText.push_front( ch );
					}
				}
			}
		}
		else
		{
			// Fits, consume char from buffer
			if( mRightToLeft )
			{
				unwrittenText.pop_back();
			}
			else
			{
				unwrittenText.pop_front();
			}
		}
	}

	// Handle right-to-left output
	if( mRightToLeft )
	{
		for( rftl::string& line : textLines )
		{
			rftl::reverse( line.begin(), line.end() );
		}
	}

	mSlotController->SetText( textLines );
	mNumCharactersUnwrittenLastRender = unwrittenText.size();
}

///////////////////////////////////////////////////////////////////////////////
}
