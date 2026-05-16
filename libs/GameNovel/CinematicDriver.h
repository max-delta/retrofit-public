#pragma once
#include "project.h"

#include "GameNovel/NovelFwd.h"

#include "GameDialogue/DialogueFwd.h"

#include "core/ptr/weak_ptr.h"
#include "core/macros.h"

#include "rftl/functional"


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
	explicit CinematicDriver(
		WeakPtr<dialogue::DialogueSequence const> sequence );

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
	void ChangeSequence( WeakPtr<dialogue::DialogueSequence const> sequence );

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
	WeakPtr<dialogue::DialogueSequence const> mSequence;
	size_t mNextEntryToProcess = 0;

	WeakPtr<ui::controller::DialogueBox> mDialogueBox;

	bool mIsRightToLeft = false;
	CinematicActions::Value mQueuedActions = CinematicActions::Invalid;
};

///////////////////////////////////////////////////////////////////////////////
}
