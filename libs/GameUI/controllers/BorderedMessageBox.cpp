#include "stdafx.h"
#include "BorderedMessageBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/TextBox.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"

#include "rftl/algorithm"


RFTYPE_CREATE_META( RF::ui::controller::BorderedMessageBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::BorderedMessageBox );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

BorderedMessageBox::BorderedMessageBox(
	size_t numRows,
	FontPurposeID purpose,
	rftl::unordered_set<char> const& breakableChars )
	: // Clang-format is trash garbage
	BorderedMessageBox(
		numRows,
		purpose,
		Justification::MiddleCenter,
		math::Color3f::kWhite,
		breakableChars )
{
	//
}



BorderedMessageBox::BorderedMessageBox(
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



void BorderedMessageBox::SetFrameTileset(
	ui::UIContext& context,
	gfx::ManagedTilesetID tileset,
	gfx::ppu::Coord expectedTileDimensions,
	gfx::ppu::Coord expectedPatternDimensions,
	gfx::ppu::Coord paddingDimensions )
{
	mFrameController->SetTileset( context, tileset, expectedTileDimensions, expectedPatternDimensions, paddingDimensions );
}



void BorderedMessageBox::SetAnimationSpeed( uint8_t charsPerFrame )
{
	mAnimSpeed = charsPerFrame;
}



ContainerID BorderedMessageBox::GetChildContainerID() const
{
	return mChildContainerID;
}



void BorderedMessageBox::SetText( rftl::string_view text, bool rightToLeft )
{
	mRightToLeft = rightToLeft;
	mText = text;
}



size_t BorderedMessageBox::GetNumCharactersDispatchedLastRender() const
{
	return mNumCharsDispatched;
}



size_t BorderedMessageBox::GetNumCharactersRenderedLastRender() const
{
	return mNumCharsRendered;
}



void BorderedMessageBox::OnInstanceAssign( UIContext& context, Container& container )
{
	mChildContainerID = CreateChildContainer(
		context.GetMutableContainerManager(),
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	WeakPtr<ui::controller::BorderFrame> const frame =
		context.GetMutableContainerManager().AssignStrongController(
			mChildContainerID,
			DefaultCreator<ui::controller::BorderFrame>::Create() );
	frame->SetJustification( ui::Justification::MiddleCenter );
	mFrameController = frame;

	WeakPtr<ui::controller::TextBox> const text =
		context.GetMutableContainerManager().AssignStrongController(
			frame->GetChildContainerID(),
			DefaultCreator<ui::controller::TextBox>::Create(
				mNumRows,
				mFontPurpose,
				mJustification,
				mColor,
				mBreakableChars ) );
	mTextController = text;
}



void BorderedMessageBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	if( mText.empty() )
	{
		mTextController->SetText( rftl::string_view(), mRightToLeft );
		return;
	}

	// TODO: This needs to better handle the truncation case, since the TextBox
	//  doesn't know that an animating word is GOING to be truncated once it
	//  finishes. Perhaps the TextBox should also take a string of characters
	//  that are non-rendering, which it uses during truncation logic, but not
	//  during render logic.

	size_t const previousNumDispatched = mNumCharsDispatched;
	size_t const previousNumUnrendered = mTextController->GetNumCharactersUnwrittenLastRender();
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

	size_t const animateLength = mAnimSpeed > 0u ? mAnimSpeed : 255u;

	size_t const numToDispatch = math::Min( mText.length(), previousNumRendered + animateLength );
	if( numToDispatch == mNumCharsDispatched )
	{
		// No change
	}
	else
	{
		if( numToDispatch == mText.length() )
		{
			mTextController->SetText( mText, mRightToLeft );
		}
		else
		{
			mTextController->SetText( mText.substr( 0, numToDispatch ), mRightToLeft );
		}
		mNumCharsDispatched = numToDispatch;
	}
}



void BorderedMessageBox::OnAABBRecalc( UIContext& context, Container& container )
{
	mAABBChanged = true;
}



void BorderedMessageBox::OnZoomFactorChange( UIContext& context, Container& container )
{
	mAABBChanged = true;
}

///////////////////////////////////////////////////////////////////////////////
}
