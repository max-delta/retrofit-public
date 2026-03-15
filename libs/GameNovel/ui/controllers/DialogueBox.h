#pragma once
#include "project.h"

#include "GameNovel/ui/UIFwd.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Color3u8.h"

#include "rftl/extension/unique_char_set.h"
#include "rftl/string_view"


// Forwards
namespace RF::ui::controller {
class ClampSlicer;
class FramePackDisplay;
class MessageBox;
}

namespace RF::novel::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Dialogue box that extends a message box with an optional portrait
class GAMENOVEL_API DialogueBox final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( DialogueBox );

	//
	// Public methods
public:
	DialogueBox() = delete;
	DialogueBox(
		gfx::ppu::CoordElem portraitWidth,
		size_t numRows,
		FontPurposeID purpose,
		rftl::unique_char_set const& breakableChars );
	DialogueBox(
		gfx::ppu::CoordElem portraitWidth,
		size_t numRows,
		FontPurposeID purpose,
		Justification::Value justification,
		math::Color3u8 color,
		rftl::unique_char_set const& breakableChars );

	void SetAnimationSpeed( uint8_t charsPerFrame );
	void SetFastForwardEvent( FocusEventType event );
	void SetContinuationEvent( FocusEventType event );

	void SetTruncationContinuationIndicator(
		gfx::ppu::ManagedFramePackID framePack,
		uint8_t maxTimeIndex,
		gfx::TimeSlowdownRate rate,
		gfx::ppu::CoordElem expectedWidth,
		gfx::ppu::CoordElem expectedHeight );
	void SetCompletionContinuationIndicator(
		gfx::ppu::ManagedFramePackID framePack,
		uint8_t maxTimeIndex,
		gfx::TimeSlowdownRate rate,
		gfx::ppu::CoordElem expectedWidth,
		gfx::ppu::CoordElem expectedHeight );

	// NOTE: Right-to-left text is parsed right-to-left, so the leftmost text
	//  is the text that will get truncated first from the string argument
	void SetText( rftl::string_view text, bool rightToLeft );

	// NOTE: Portrait is hidden and unset by default
	void SetPortrait( gfx::ppu::ManagedFramePackID framePack, uint8_t maxTimeIndex, gfx::TimeSlowdownRate rate );
	void ShowPortrait( UIContext& context, Justification::Value portraitSide, bool flipHorizontal );
	void HidePortrait( UIContext& context );

	void ReflowAllText();

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnAddedToFocusTree( UIContext& context, FocusTreeNode& newNode ) override final;


	//
	// Private data
private:
	size_t const mNumRows;
	FontPurposeID const mFontPurpose;
	Justification::Value const mJustification;
	math::Color3u8 const mColor;
	gfx::ppu::CoordElem const mPortraitWidth;
	rftl::unique_char_set const mBreakableChars;

	ContainerID mSliceContainerID = kInvalidContainerID;
	WeakPtr<ClampSlicer> mSliceController;
	WeakPtr<FramePackDisplay> mPortraitController;
	WeakPtr<MessageBox> mMessageController;

	rftl::string mText;
};

///////////////////////////////////////////////////////////////////////////////
}
