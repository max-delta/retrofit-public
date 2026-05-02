#pragma once
#include "project.h"

#include "GameDialogue/DialogueEntry.h"

#include "core/macros.h"

#include "rftl/deque"
#include "rftl/string"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

// A full dialogue sequence that is ready for execution, in contrast to the
// 	intermediate forms that are only partially processed
// IMPORTANT: Unlike the intermediate forms, this stores text data, so does not
//  need the original buffers to remain in place after it's been created
class GAMEDIALOGUE_API DialogueSequence final
{
	RF_NO_COPY( DialogueSequence );
	RF_DEFAULT_MOVE( DialogueSequence );

	//
	// Types and constants
public:
	using Entry = DialogueEntry;
	using Entries = rftl::deque<Entry>;

	// Because many strings are re-used across multiple entries, we keep the
	//  actual text data in shared storage
	using Strings = rftl::deque<rftl::string>;


	//
	// Public methods
public:
	DialogueSequence();
	~DialogueSequence();


	//
	// Public data
public:
	rftl::string mLayout;

	// All the data that will be needed to process the sequence fully, which
	//  can be quickly checked in advance for correctness and compatibility,
	//  allowing the executor to error out or warn before actual going through
	//  the entries
	// NOTE: There is an implicit contractual promise here that these strings
	//  will have the same memory as those that appear later in entries, so an
	//  executor could pre-process the strings to its own identifier concepts,
	//  and then do pointer comparisons against the entries instead of full
	//  string comparisons (since the string comparisons would be done on these
	//  shared cache values instead)
	Strings mRequiredCommands;
	Strings mRequiredCommandParamKeys;
	Strings mRequiredCommandParamVals;
	Strings mRequiredScenes;
	Strings mRequiredSceneParamKeys;
	Strings mRequiredSceneParamVals;
	Strings mRequiredConditionKeys;
	Strings mRequiredConditionVals;
	Strings mRequiredCharacters;
	Strings mRequiredExpressions;
	Strings mSpeechStorage;

	Entries mEntries;
};

///////////////////////////////////////////////////////////////////////////////
}
