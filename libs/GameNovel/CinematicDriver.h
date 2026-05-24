#pragma once
#include "project.h"

#include "GameNovel/NovelFwd.h"

#include "GameDialogue/DialogueFwd.h"

#include "PPU/FramePackRef.h"

#include "core/ptr/weak_ptr.h"
#include "core/macros.h"

#include "rftl/functional"
#include "rftl/unordered_map"
#include "rftl/string_view"


namespace RF::novel::ui::controller {
class DialogueBox;
}

namespace RF::novel {
///////////////////////////////////////////////////////////////////////////////

// Uses a dialogue sequence to drive UI elements that are normally associated
//  with cutscenes, cinematics, and visual novels
class GAMENOVEL_API CinematicDriver
{
	RF_NO_COPY( CinematicDriver );

	//
	// Types and constants
public:
	using OnEntrySig = void( dialogue::DialogueEntry const& entry );
	using OnEntryFunc = rftl::function<OnEntrySig>;

	// NOTE: These are string views, because they are expected to be backed by
	//  the strings in the sequence, and thus do not need an extra allocation
	using FramePackByExpression = rftl::unordered_map<rftl::string_view, gfx::ppu::FramePackRef>;
	using FramePacksByCharacter = rftl::unordered_map<rftl::string_view, FramePackByExpression>;

	struct SequenceParams
	{
		WeakPtr<dialogue::DialogueSequence const> mSequence;
		FramePacksByCharacter mFramePacksByCharacter;
	};

	struct TickParams
	{
		OnEntryFunc mOnCommand = nullptr;
		OnEntryFunc mOnScene = nullptr;
		OnEntryFunc mOnSpeech = nullptr;
	};

	//
	// Public methods
public:
	CinematicDriver() = delete;
	explicit CinematicDriver( SequenceParams&& sequenceParams );

	// Examines the current state of the cinematic
	CinematicState EvaluateCurrentState() const;

	// Queue up actions to take on the next tick
	void QueueActions( CinematicActions::Value actions );

	// Ticks the cinematic, performing any operations that are queued up or
	//  take multiple frames to progress through
	void TickCinematic( TickParams& params );
	void TickCinematic();

	// Change the sequence and cause a harsh reset, as it is impractical to try
	//  and collision-resolve an incoming sequence with the state and
	//  progression of the current sequence
	void ChangeSequence( SequenceParams&& sequenceParams );

	// Essentially performs a complete rewind back to the head of the sequence
	void ResetProgression();

	// This dialogue box will be driven to emit the text and portraiture
	// NOTE: After setting, the dialogue box will not automatically update
	//  to the most recent text, that will require an advance, so it is
	//  expected that this is set before the cinematic really begins
	void SetDialogueBox( WeakPtr<ui::controller::DialogueBox> dialogueBox );
	void UnsetDialogueBox();


	//
	// Private methods
private:
	// Returns true if can be sub-ticked again
	bool SubTickCinematic( TickParams& params );
	bool SubTickCinematic_Advance( TickParams& params );
	bool SubTickCinematic_Advance_Command( TickParams& params, dialogue::DialogueEntry const& entry );
	bool SubTickCinematic_Advance_Scene( TickParams& params, dialogue::DialogueEntry const& entry );
	bool SubTickCinematic_Advance_Speech( TickParams& params, dialogue::DialogueEntry const& entry );

	dialogue::DialogueEntry const& ConsumeNextEntry();


	//
	// Private data
private:
	SequenceParams mSequenceParams;
	size_t mNextEntryToProcess = 0;

	WeakPtr<ui::controller::DialogueBox> mDialogueBox;

	bool mIsRightToLeft = false;
	CinematicActions::Value mQueuedActions = CinematicActions::Invalid;
};

///////////////////////////////////////////////////////////////////////////////
}
