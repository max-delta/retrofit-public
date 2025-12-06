#pragma once
#include "project.h"

#include "GameDialogue/DialogueFwd.h"

#include "core/macros.h"

#include "rftl/deque"
#include "rftl/string_view"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

// Processes raw dialogue data into the form that can be used at runtime
class GAMEDIALOGUE_API DialogueLoader final
{
	RF_NO_INSTANCE( DialogueLoader );

	//
	// Types and constants
public:
	using RawEntries = rftl::deque<RawDialogueEntry>;


	//
	// Public methods
public:
	static RawEntries FirstPassParse( rftl::string_view rawText );
	static DialogueSequence SecondPassParse( RawEntries const& rawEntries );

	static DialogueSequence Parse( rftl::string_view rawText );
};

///////////////////////////////////////////////////////////////////////////////
}
