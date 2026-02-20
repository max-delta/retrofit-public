#include "stdafx.h"
#include "MessageBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/meta/ScopedCleanup.h"
#include "core/ptr/default_creator.h"

#include "rftl/algorithm"


RFTYPE_CREATE_META( RF::ui::controller::MessageBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::MessageBox );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr bool kShowMessageBoxDebug = true;

}
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
	math::Color3u8 color,
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



void MessageBox::SetContinuationEvent( FocusEventType event )
{
	mContinuationEvent = event;
}



void MessageBox::SetText( rftl::string_view text, bool rightToLeft )
{
	mFullText = text;
	mNumCharsSkipped = 0;

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



void MessageBox::ReflowAllText()
{
	mReflowOnNextFrame = true;
	mState = State::Animating;
}



void MessageBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	// Make sure to render text at the end of it all
	auto const onScopeEndParentRender = RF::OnScopeEnd(
		[this, &context, &container, &blockChildRendering]() -> void
		{
			TextBox::OnRender( context, container, blockChildRendering );
		} );

	// For displaying things at end of message
	static constexpr auto findEndOfMessage = []( TextBox const& self ) -> rftl::optional<gfx::ppu::Coord>
	{
		rftl::vector<gfx::ppu::AABB> const lastAABBs = self.GetTextAABBsUsedLastRender();

		// Ignore empty AABBs
		rftl::optional<gfx::ppu::AABB> lastNonEmptyAABB;
		RF_ASSERT_MSG( lastAABBs.empty() == false, "No text?" );
		for( gfx::ppu::AABB const& aabb : lastAABBs )
		{
			if( aabb.Width() <= 0 )
			{
				continue;
			}
			lastNonEmptyAABB = aabb;
		}
		if( lastNonEmptyAABB.has_value() == false )
		{
			return rftl::nullopt;
		}
		gfx::ppu::AABB const& lastAABB = lastNonEmptyAABB.value();

		// Where is the end point?
		gfx::ppu::Coord const lastCoord = {
			self.IsRightToLeft() ? lastAABB.Left() : lastAABB.Right(),
			lastAABB.Bottom() };
		return lastCoord;
	};

	// For diagnostic use only
	auto const drawEndOfMessageDebug = [this, &context, &container]() -> void
	{
		if constexpr( details::kShowMessageBoxDebug == false )
		{
			return;
		}

		gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

		rftl::optional<gfx::ppu::Coord> lastCoord = findEndOfMessage( *this );
		if( lastCoord.has_value() == false )
		{
			renderer.DebugDrawText( container.mAABB.mBottomRight, "X" );
			return;
		}

		RF_ASSERT( mState != State::Invalid );
		switch( mState )
		{
			case State::Animating:
				renderer.DebugDrawText( lastCoord.value(), ">" );
				break;
			case State::Truncated:
				renderer.DebugDrawText( lastCoord.value(), "+" );
				break;
			case State::Completed:
				renderer.DebugDrawText( lastCoord.value(), "#" );
				break;
			case State::Invalid:
			default:
				renderer.DebugDrawText( lastCoord.value(), "X" );
				break;
		}
	};

	// If waiting for user input, we can display an indicator of such
	auto const drawEndOfMessageIndicator = [this, &context]( int TODO ) -> void
	{
		rftl::optional<gfx::ppu::Coord> lastCoord = findEndOfMessage( *this );
		if( lastCoord.has_value() == false )
		{
			RF_DBGFAIL_MSG( "Empty text?" );
			return;
		}

		RF_TODO_ANNOTATION( "Actual sprite, configurable" );
		( (void)TODO );
		gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );
		renderer.DebugDrawText( lastCoord.value(), "_!" );
	};

	// Conditionally display the continuation indicators after the main logic
	auto const onScopeEndMessageIndicator = RF::OnScopeEnd(
		[this, &drawEndOfMessageIndicator]() -> void
		{
			RF_ASSERT( mState != State::Invalid );
			if( mState == State::Truncated )
			{
				drawEndOfMessageIndicator( 0 );
			}
			else if( mState == State::Completed )
			{
				drawEndOfMessageIndicator( 1 );
			}
		} );

	// Draw debug position logic
	drawEndOfMessageDebug();

	bool const rightToLeft = IsRightToLeft();

	if( mFullText.empty() )
	{
		mNumCharsDispatched = 0;
		mNumCharsRendered = 0;
		TextBox::SetText( rftl::string_view(), rightToLeft );
		mState = State::Completed;
		return;
	}

	if( mReflowOnNextFrame )
	{
		mNumCharsSkipped = 0;
		mNumCharsDispatched = 0;
		mNumCharsRendered = 0;
		TextBox::SetText( rftl::string_view(), rightToLeft );
		mReflowOnNextFrame = false;
		mState = State::Animating;
		return;
	}

	// TODO: This needs to better handle the truncation case, since the TextBox
	//  doesn't know that an animating word is GOING to be truncated once it
	//  finishes. Perhaps the TextBox should also take a string of characters
	//  that are non-rendering, which it uses during truncation logic, but not
	//  during render logic.

	size_t previousNumDispatched = mNumCharsDispatched;
	size_t previousNumUnrendered = GetNumCharactersUnwrittenLastRender();
	RF_ASSERT( previousNumDispatched >= previousNumUnrendered );
	size_t previousNumRendered = previousNumDispatched - previousNumUnrendered;

	mNumCharsRendered = previousNumRendered;

	// Are we truncated and trying to advance?
	if( mState == State::Truncated && mContinueOnNextFrame )
	{
		RF_ASSERT( mNumCharsSkipped + previousNumRendered < mFullText.size() );
		mNumCharsSkipped += previousNumRendered;
		previousNumDispatched = 0;
		previousNumUnrendered = 0;
		previousNumRendered = 0;
		mState = State::Animating;
	}
	mContinueOnNextFrame = false;

	// May be skipping some characters
	rftl::string_view const currentFlowAttempt =
		rftl::string_view( mFullText ).substr( mNumCharsSkipped );

	if( mState == State::Truncated && mAABBChanged == false )
	{
		// Don't try to animate
		return;
	}
	else if( mAABBChanged )
	{
		// Lift block and try to animate
		mAABBChanged = false;
		mState = State::Animating;
	}
	else if( previousNumUnrendered > 0 )
	{
		// Stop animating, no more room
		mState = State::Truncated;
		return;
	}
	else if( previousNumRendered < currentFlowAttempt.size() )
	{
		// Try to animate
		RF_ASSERT( mState == State::Animating );
	}
	else if( mState == State::Completed )
	{
		// Don't animate, already completed
		RF_ASSERT( mAABBChanged == false );
		return;
	}
	else
	{
		// Stop animating, completed
		RF_ASSERT( mState == State::Animating );
		RF_ASSERT( previousNumUnrendered == 0 );
		RF_ASSERT( mNumCharsSkipped + previousNumRendered == mFullText.size() );
		mState = State::Completed;
		return;
	}

	// Figure out how much to dispatch
	size_t const attempLength = currentFlowAttempt.length();
	size_t numToDispatch = math::Min( attempLength, previousNumRendered + mAnimSpeed );
	bool const fastForward = mFastForwardOnNextFrame || mAnimSpeed == 0u;
	if( fastForward )
	{
		numToDispatch = attempLength;
	}
	mFastForwardOnNextFrame = false;

	if( numToDispatch == previousNumDispatched )
	{
		// No change
	}
	else
	{
		if( numToDispatch == currentFlowAttempt.length() )
		{
			TextBox::SetText( currentFlowAttempt, rightToLeft );
		}
		else
		{
			TextBox::SetText( currentFlowAttempt.substr( 0, numToDispatch ), rightToLeft );
		}
		mNumCharsDispatched = numToDispatch;
	}
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
		bool const stillHasUndispatchedText = GetNumCharactersDispatchedLastRender() < mFullText.size();
		bool const canFastForward = stillHasUndispatchedText && mState == State::Animating;
		if( canFastForward )
		{
			mFastForwardOnNextFrame = true;
			handled = true;
			// NOTE: Not returning, since we're not sure what the event is or
			//  if it will conflict with other events we're looking for
		}
	}

	if( focusEvent.mEventType == mContinuationEvent )
	{
		bool const stillHasUnseenText = mNumCharsSkipped + GetNumCharactersRenderedLastRender() < mFullText.size();
		bool const canContinue = stillHasUnseenText && mState == State::Truncated;
		if( canContinue )
		{
			mContinueOnNextFrame = true;
			handled = true;
			// NOTE: Not returning, since we're not sure what the event is or
			//  if it will conflict with other events we're looking for
		}
	}

	return handled;
}

///////////////////////////////////////////////////////////////////////////////
}
