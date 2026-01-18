#include "stdafx.h"
#include "MessageBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"

#include "rftl/algorithm"


RFTYPE_CREATE_META( RF::ui::controller::MessageBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::MessageBox );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

MessageBox::MessageBox(
	size_t numRows,
	FontPurposeID purpose,
	rftl::unordered_set<char> const& breakableChars )
	: // Clang-format is trash garbage
	TextBox(
		numRows,
		purpose,
		breakableChars )
{
	//
}



MessageBox::MessageBox(
	size_t numRows,
	FontPurposeID purpose,
	Justification::Value justification,
	math::Color3f color,
	rftl::unordered_set<char> const& breakableChars )
	: // Clang-format is trash garbage
	TextBox(
		numRows,
		purpose,
		justification,
		color,
		breakableChars )
{
	//
}



void MessageBox::SetAnimationSpeed( uint8_t charsPerFrame )
{
	mAnimSpeed = charsPerFrame;
}



void MessageBox::SetFastForwardEvent( FocusEventType event )
{
	mFastForwardEvent = event;
}



void MessageBox::SetText( rftl::string_view text, bool rightToLeft )
{
	mFullText = text;

	TextBox::SetText( {}, rightToLeft );
}



size_t MessageBox::GetNumCharactersDispatchedLastRender() const
{
	return mNumCharsDispatched;
}



size_t MessageBox::GetNumCharactersRenderedLastRender() const
{
	return mNumCharsRendered;
}



void MessageBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	bool const rightToLeft = IsRightToLeft();

	if( mFullText.empty() )
	{
		TextBox::SetText( rftl::string_view(), rightToLeft );
		return;
	}

	// TODO: This needs to better handle the truncation case, since the TextBox
	//  doesn't know that an animating word is GOING to be truncated once it
	//  finishes. Perhaps the TextBox should also take a string of characters
	//  that are non-rendering, which it uses during truncation logic, but not
	//  during render logic.

	size_t const previousNumDispatched = mNumCharsDispatched;
	size_t const previousNumUnrendered = GetNumCharactersUnwrittenLastRender();
	RF_ASSERT( previousNumDispatched >= previousNumUnrendered );
	size_t const previousNumRendered = previousNumDispatched - previousNumUnrendered;

	mNumCharsRendered = previousNumRendered;

	if( mBlockAnimUntilAABBChange && mAABBChanged == false )
	{
		// Don't try to animate
		return;
	}
	else if( mAABBChanged )
	{
		// Lift block and try to animate
		mAABBChanged = false;
		mBlockAnimUntilAABBChange = false;
	}
	else if( previousNumUnrendered > 0 )
	{
		// Stop animating
		mBlockAnimUntilAABBChange = true;
		return;
	}
	else
	{
		// Try to animate
	}

	// Figure out how much to dispatch
	size_t const fullTextLength = mFullText.length();
	size_t numToDispatch = math::Min( fullTextLength, previousNumRendered + mAnimSpeed );
	bool const fastForward = mFastForwardOnNextFrame || mAnimSpeed == 0u;
	if( fastForward )
	{
		numToDispatch = fullTextLength;
	}
	mFastForwardOnNextFrame = false;

	if( numToDispatch == mNumCharsDispatched )
	{
		// No change
	}
	else
	{
		if( numToDispatch == mFullText.length() )
		{
			TextBox::SetText( mFullText, rightToLeft );
		}
		else
		{
			TextBox::SetText( mFullText.substr( 0, numToDispatch ), rightToLeft );
		}
		mNumCharsDispatched = numToDispatch;
	}

	TextBox::OnRender( context, container, blockChildRendering );
}



void MessageBox::OnAABBRecalc( UIContext& context, Container& container )
{
	mAABBChanged = true;

	TextBox::OnAABBRecalc( context, container );
}



void MessageBox::OnZoomFactorChange( UIContext& context, Container& container )
{
	mAABBChanged = true;

	TextBox::OnZoomFactorChange( context, container );
}



bool MessageBox::OnFocusEvent( UIContext& context, FocusEvent const& focusEvent )
{
	bool handled = false;

	if( focusEvent.mEventType == mFastForwardEvent )
	{
		bool const canFastForward = GetNumCharactersDispatchedLastRender() < mFullText.size();
		if( canFastForward )
		{
			mFastForwardOnNextFrame = true;
			handled = true;
			// NOTE: Not returning, since we're not sure what the event is or
			//  if it will conflict with other events we're looking for
		}
	}

	return handled;
}

///////////////////////////////////////////////////////////////////////////////
}
