#include "stdafx.h"
#include "DialogueBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusTree.h"
#include "GameUI/controllers/ClampSlicer.h"
#include "GameUI/controllers/FramePackDisplay.h"
#include "GameUI/controllers/MessageBox.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::novel::ui::controller::DialogueBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::novel::ui::controller::DialogueBox );
}

namespace RF::novel::ui::controller {
///////////////////////////////////////////////////////////////////////////////

DialogueBox::DialogueBox(
	gfx::ppu::CoordElem portraitWidth,
	size_t numRows,
	FontPurposeID purpose,
	rftl::unique_char_set const& breakableChars,
	rftl::unique_char_set const& consumableChars )
	: // Clang-format is trash garbage
	DialogueBox(
		portraitWidth,
		numRows,
		purpose,
		Justification::MiddleCenter,
		math::Color3u8::kWhite,
		breakableChars,
		consumableChars )
{
	//
}



DialogueBox::DialogueBox(
	gfx::ppu::CoordElem portraitWidth,
	size_t numRows,
	FontPurposeID purpose,
	Justification::Value justification,
	math::Color3u8 color,
	rftl::unique_char_set const& breakableChars,
	rftl::unique_char_set const& consumableChars )
	: mNumRows( numRows )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mColor( color )
	, mPortraitWidth( portraitWidth )
	, mBreakableChars( breakableChars )
	, mConsumableChars( consumableChars )
{
	//
}



void DialogueBox::SetAnimationSpeed( uint8_t charsPerFrame )
{
	RF_ASSERT( mMessageController != nullptr );
	mMessageController->SetAnimationSpeed( charsPerFrame );
}



void DialogueBox::SetFastForwardEvent( FocusEventType event )
{
	RF_ASSERT( mMessageController != nullptr );
	mMessageController->SetFastForwardEvent( event );
}



void DialogueBox::SetContinuationEvent( FocusEventType event )
{
	RF_ASSERT( mMessageController != nullptr );
	mMessageController->SetContinuationEvent( event );
}



void DialogueBox::SetTruncationContinuationIndicator(
	gfx::ppu::ManagedFramePackID framePack,
	uint8_t maxTimeIndex,
	gfx::TimeSlowdownRate rate,
	gfx::ppu::CoordElem expectedWidth,
	gfx::ppu::CoordElem expectedHeight )
{
	RF_ASSERT( mMessageController != nullptr );
	mMessageController->SetTruncationContinuationIndicator(
		framePack,
		maxTimeIndex,
		rate,
		expectedWidth,
		expectedHeight );
}



void DialogueBox::SetCompletionContinuationIndicator(
	gfx::ppu::ManagedFramePackID framePack,
	uint8_t maxTimeIndex,
	gfx::TimeSlowdownRate rate,
	gfx::ppu::CoordElem expectedWidth,
	gfx::ppu::CoordElem expectedHeight )
{
	RF_ASSERT( mMessageController != nullptr );
	mMessageController->SetCompletionContinuationIndicator(
		framePack,
		maxTimeIndex,
		rate,
		expectedWidth,
		expectedHeight );
}



void DialogueBox::SetText( rftl::string_view text, bool rightToLeft )
{
	RF_ASSERT( mMessageController != nullptr );
	mMessageController->SetText( text, rightToLeft );
}



void DialogueBox::SetPortrait( gfx::ppu::ManagedFramePackID framePack, uint8_t maxTimeIndex, gfx::TimeSlowdownRate rate )
{
	mPortraitController->SetFramePack( framePack, maxTimeIndex, mPortraitWidth, mPortraitWidth );
	mPortraitController->SetSlowdown( rate );
}



void DialogueBox::ShowPortrait( UIContext& context, Justification::Value portraitSide, bool flipHorizontal )
{
	mPortraitController->SetRenderingBlocked( false );
	mPortraitController->SetHorizontalFlip( flipHorizontal );

	RF_ASSERT(
		portraitSide == Justification::Value::Left ||
		portraitSide == Justification::Value::Right );
	ClampSlicer::Mode const mode =
		portraitSide == Justification::Value::Left ?
		ClampSlicer::Mode::ClampLeft_OverflowRight :
		ClampSlicer::Mode::ClampRight_OverflowLeft;
	mSliceController->SetMode( context, mode );
}



void DialogueBox::HidePortrait( UIContext& context )
{
	mPortraitController->SetRenderingBlocked( true );
	mSliceController->SetMode( context, ClampSlicer::Mode::TotalOverlap );
}



void DialogueBox::ReflowAllText()
{
	RF_ASSERT( mMessageController != nullptr );
	mMessageController->ReflowAllText();
}



void DialogueBox::OnInstanceAssign( UIContext& context, Container& container )
{
	ContainerManager& uiManager = context.GetMutableContainerManager();

	mSliceContainerID = CreateChildContainer(
		uiManager,
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	WeakPtr<ClampSlicer> const slicer =
		uiManager.AssignStrongController(
			mSliceContainerID,
			DefaultCreator<ClampSlicer>::Create(
				ClampSlicer::Params{ .max = mPortraitWidth },
				ClampSlicer::Mode::ClampLeft_OverflowRight ) );
	mSliceController = slicer;

	WeakPtr<FramePackDisplay> const portrait =
		uiManager.AssignStrongController(
			slicer->GetClampedContainerID(),
			DefaultCreator<FramePackDisplay>::Create() );
	mPortraitController = portrait;

	WeakPtr<MessageBox> const message =
		uiManager.AssignStrongController(
			slicer->GetOverflowContainerID(),
			DefaultCreator<MessageBox>::Create(
				mNumRows,
				mFontPurpose,
				ui::Justification::MiddleLeft,
				mColor,
				mBreakableChars,
				mConsumableChars ) );
	mMessageController = message;

	// Start hidden by default, since we also need a valid framepack before we
	//  can flip it on
	HidePortrait( context );
}



void DialogueBox::OnAddedToFocusTree( UIContext& context, FocusTreeNode& newNode )
{
	// Add message box to tree
	mMessageController->AddAsChildToFocusTreeNode( context, newNode );
}

///////////////////////////////////////////////////////////////////////////////
}
