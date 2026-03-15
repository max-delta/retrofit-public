#include "stdafx.h"
#include "DialogueBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusTree.h"
#include "GameUI/controllers/ClampSlicer.h"
#include "GameUI/controllers/MessageBox.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::novel::ui::controller::DialogueBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::novel::ui::controller::DialogueBox );
}

namespace RF::novel::ui::controller {
///////////////////////////////////////////////////////////////////////////////
namespace details {

//

}
///////////////////////////////////////////////////////////////////////////////

DialogueBox::DialogueBox(
	gfx::ppu::CoordElem portraitWidth,
	size_t numRows,
	FontPurposeID purpose,
	rftl::unique_char_set const& breakableChars )
	: // Clang-format is trash garbage
	DialogueBox(
		portraitWidth,
		numRows,
		purpose,
		Justification::MiddleCenter,
		math::Color3u8::kWhite,
		breakableChars )
{
	//
}



DialogueBox::DialogueBox(
	gfx::ppu::CoordElem portraitWidth,
	size_t numRows,
	FontPurposeID purpose,
	Justification::Value justification,
	math::Color3u8 color,
	rftl::unique_char_set const& breakableChars )
	: mNumRows( numRows )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mColor( color )
	, mPortraitWidth( portraitWidth )
	, mBreakableChars( breakableChars )
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

	WeakPtr<MessageBox> const message =
		uiManager.AssignStrongController(
			slicer->GetOverflowContainerID(),
			DefaultCreator<MessageBox>::Create(
				mNumRows,
				mFontPurpose,
				ui::Justification::MiddleLeft,
				mColor,
				mBreakableChars ) );
	mMessageController = message;
}



void DialogueBox::OnAddedToFocusTree( UIContext& context, FocusTreeNode& newNode )
{
	// Add message box to tree
	mMessageController->AddAsChildToFocusTreeNode( context, newNode );
}

///////////////////////////////////////////////////////////////////////////////
}
