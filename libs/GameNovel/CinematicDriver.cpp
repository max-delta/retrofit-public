#include "stdafx.h"
#include "CinematicDriver.h"

#include "GameNovel/ui/controllers/DialogueBox.h"

#include "GameDialogue/DialogueSequence.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"

#include "core/meta/BitwiseEnums.h"

#include "rftl/extension/algorithms.h"


namespace RF::novel {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CallableT, typename... ArgsT>
void InvokeIfSet( CallableT& callable, ArgsT&&... args )
{
	if( callable )
	{
		callable( args... );
	}
}



bool ValidateCritialSequenceParams( CinematicDriver::SequenceParams const& params )
{
	if( params.mSequence == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}
	dialogue::DialogueSequence const& sequence = *params.mSequence;

	// String views are required to be backed by the sequence, so that we can
	//  be more confident that they probably aren't going to suddenly get
	//  ripped out from underneath us
	{
		using FramePackByExpression = CinematicDriver::FramePackByExpression;
		using FramePacksByCharacter = CinematicDriver::FramePacksByCharacter;
		using Strings = dialogue::DialogueSequence::Strings;
		Strings const& characterStorage = sequence.mRequiredCharacters;
		Strings const& expressionStorage = sequence.mRequiredExpressions;

		// For each character...
		for( FramePacksByCharacter::value_type const& charEntry : params.mFramePacksByCharacter )
		{
			rftl::string_view const& character = charEntry.first;
			FramePackByExpression const& expressions = charEntry.second;

			if( rftl::contains( characterStorage, character ) == false )
			{
				RF_DBGFAIL_MSG( "Character reference not backed by sequence memory" );
				return false;
			}

			// For each expresion...
			for( FramePackByExpression::value_type const& exprEntry : expressions )
			{
				rftl::string_view const& expression = exprEntry.first;

				if( rftl::contains( expressionStorage, expression ) == false )
				{
					RF_DBGFAIL_MSG( "Expression reference not backed by sequence memory" );
					return false;
				}
			}
		}
	}

	return true;
}

}
///////////////////////////////////////////////////////////////////////////////

CinematicDriver::CinematicDriver( SequenceParams&& sequenceParams )
	: mSequenceParams( rftl::move( sequenceParams ) )
{
	RF_ASSERT( mSequenceParams.mSequence != nullptr );

	if( details::ValidateCritialSequenceParams( mSequenceParams ) == false )
	{
		RF_DBGFAIL();
		mSequenceParams = {};
	}

	ResetProgression();
}



CinematicState CinematicDriver::EvaluateCurrentState() const
{
	RF_ASSERT( mSequenceParams.mSequence != nullptr );
	dialogue::DialogueSequence const& sequence = *mSequenceParams.mSequence;

	size_t const numEntries = sequence.mEntries.size();
	RF_ASSERT( mNextEntryToProcess <= numEntries );

	// At end of sequence?
	if( mNextEntryToProcess >= numEntries )
	{
		return CinematicState::EndOfSequence;
	}

	// Assume we're just waiting to advance
	return CinematicState::WaitingForAdvance;
}



void CinematicDriver::QueueActions( CinematicActions::Value actions )
{
	using namespace enable_bitwise_enums;

	if( ( mQueuedActions & actions ) != 0 )
	{
		RFLOG_WARNING( nullptr, RFCAT_GAMENOVEL, "Double-queued a cinematic action: {}", math::enum_bitcast( actions ) );
	}

	mQueuedActions |= actions;
}



void CinematicDriver::TickCinematic( TickParams& params )
{
	while( SubTickCinematic( params ) )
	{
		// Sub-tick until complete
	}
}



void CinematicDriver::TickCinematic()
{
	TickParams unused = {};
	TickCinematic( unused );
}



void CinematicDriver::ChangeSequence( SequenceParams&& sequenceParams )
{
	RF_ASSERT( sequenceParams.mSequence != nullptr );
	mSequenceParams = rftl::move( sequenceParams );

	if( details::ValidateCritialSequenceParams( mSequenceParams ) == false )
	{
		RF_DBGFAIL();
		mSequenceParams = {};
	}

	ResetProgression();
}



void CinematicDriver::ResetProgression()
{
	// The next entry to process will be the first, meaning our 'current' entry
	//  is conceptually undefined
	mNextEntryToProcess = 0;

	// Clear any existing actions, since we are doing a big fundamental state
	//  change, and it's dubious whether any are still relevant
	mQueuedActions = {};

	// Since we're on an undefined entry right now, the first thing we should
	//  do when we get a chance is to advance into the first entry
	QueueActions( CinematicActions::Advance );
}



void CinematicDriver::SetDialogueBox( WeakPtr<ui::controller::DialogueBox> dialogueBox )
{
	RF_ASSERT( dialogueBox != nullptr );
	mDialogueBox = dialogueBox;

	mDialogueBox->SetText( "UNSET", false );
}



void CinematicDriver::UnsetDialogueBox()
{
	mDialogueBox = nullptr;
}

///////////////////////////////////////////////////////////////////////////////

bool CinematicDriver::SubTickCinematic( TickParams& params )
{
	CinematicState const currentState = EvaluateCurrentState();

	switch( currentState )
	{
		case CinematicState::WaitingForAdvance:
			return SubTickCinematic_Advance( params );
		case CinematicState::EndOfSequence:
			return false;
		case CinematicState::Invalid:
		default:
			RF_DBGFAIL();
			return false;
	}
}



bool CinematicDriver::SubTickCinematic_Advance( TickParams& params )
{
	if( ( mQueuedActions & CinematicActions::Advance ) == 0 )
	{
		// An advance isn't available yet
		return false;
	}

	// Consume the entry and advance
	auto const consume = [this, &params]() -> bool
	{
		dialogue::DialogueEntry const& entry = ConsumeNextEntry();
		switch( entry.mEntryType )
		{
			case dialogue::EntryType::Command:
				return SubTickCinematic_Advance_Command( params, entry );
			case dialogue::EntryType::Scene:
				return SubTickCinematic_Advance_Scene( params, entry );
			case dialogue::EntryType::Speech:
				return SubTickCinematic_Advance_Speech( params, entry );
			case dialogue::EntryType::Invalid:
			default:
				RF_DBGFAIL();
				return false;
		}
	};
	bool const allowsMoreSubticking = consume();

	if( allowsMoreSubticking == false )
	{
		// Consume the advance action
		using namespace enable_bitwise_enums;
		mQueuedActions &= ~CinematicActions::Advance;
	}

	return allowsMoreSubticking;
}

bool CinematicDriver::SubTickCinematic_Advance_Command( TickParams& params, dialogue::DialogueEntry const& entry )
{
	// Optional caller hook
	details::InvokeIfSet( params.mOnCommand, entry );

	// Nothing currently

	// Allow further sub-ticking
	return true;
}

bool CinematicDriver::SubTickCinematic_Advance_Scene( TickParams& params, dialogue::DialogueEntry const& entry )
{
	// Optional caller hook
	details::InvokeIfSet( params.mOnScene, entry );

	RF_TODO_ANNOTATION( "Change the scene" );

	// Allow further sub-ticking
	return true;
}

bool CinematicDriver::SubTickCinematic_Advance_Speech( TickParams& params, dialogue::DialogueEntry const& entry )
{
	// Optional caller hook
	details::InvokeIfSet( params.mOnSpeech, entry );

	if( mDialogueBox == nullptr )
	{
		// Dubious, but will keep sub-ticking
		RFLOG_WARNING( nullptr, RFCAT_GAMENOVEL, "No dialogue box is set, skipping speech: '{}'", entry.mFallbackText );
		return true;
	}
	ui::controller::DialogueBox& dialogueBox = *mDialogueBox;

	RF_TODO_ANNOTATION( "Change the dialogue box's portrait" );
	RF_TODO_ANNOTATION( "Lookup localization key" );
	dialogueBox.SetText( entry.mFallbackText, mIsRightToLeft );

	// Stop sub-ticking
	return false;
}



dialogue::DialogueEntry const& CinematicDriver::ConsumeNextEntry()
{
	RF_ASSERT( mSequenceParams.mSequence != nullptr );
	dialogue::DialogueSequence const& sequence = *mSequenceParams.mSequence;

	dialogue::DialogueEntry const& retVal = sequence.mEntries.at( mNextEntryToProcess );
	mNextEntryToProcess++;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
