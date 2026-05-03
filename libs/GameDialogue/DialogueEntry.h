#pragma once
#include "project.h"

#include "GameDialogue/DialogueFwd.h"

#include "rftl/string_view"
#include "rftl/unordered_map"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

// A full dialogue entry that is ready for execution, in contrast to the
// 	intermediate forms that are only partially processed
// IMPORTANT: This doesn't store the actual characters, and is normally
//  expected to be tied to a sequence which is storying the actual backing data
// SEE: DialogueSequence
class GAMEDIALOGUE_API DialogueEntry final
{
	//
	// Types and constants
public:
	using KVPairs = rftl::unordered_map<rftl::string_view, rftl::string_view>;
	using KVPair = KVPairs::value_type;

	//
	// Public data
public:

	// The type, which determines how fields are interpreted
	EntryType mEntryType = EntryType::Invalid;

	// Speech, if localization available
	LocIDLineNum mLocIDLineNumber = kInvalidLocIDLineNum;

	// CommandID \ SceneID \ CharacterID
	rftl::string_view mPrimary;

	// ExpressionID
	rftl::string_view mExpression;

	// Speech, if a localization replacement was not found
	rftl::string_view mFallbackText;

	// Command params \ scene params
	KVPairs mParams;

	// Conditions that must be true / false, otherwise skip this entry
	// NOTE: This is what conditional jumps turn into
	KVPairs mNecessaryConditions;
	KVPairs mForbiddenConditions;
};

///////////////////////////////////////////////////////////////////////////////
}
