#pragma once
#include "project.h"

#include "GameDialogue/DialogueFwd.h"

#include "core/macros.h"

#include "rftl/string_view"
#include "rftl/unordered_map"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

// The raw tokenized data from a line in a dialogue file
// IMPORTANT: This doesn't store the actual characters, only the pointer data
//  that resulted from tokenization, so the backing character buffer must
//  remain present past the lifetime of this structure
class GAMEDIALOGUE_API RawDialogueEntry final
{
	RF_NO_COPY( RawDialogueEntry );
	RF_DEFAULT_MOVE( RawDialogueEntry );

	//
	// Types and constants
public:
	using KVPairs = rftl::unordered_map<rftl::string_view, rftl::string_view>;


	//
	// Public methods
public:
	RawDialogueEntry();

	static RawDialogueEntry FromLine( size_t lineNumber, rftl::string_view str );


	//
	// Public data
public:
	// From the file, the actual line number, not to be confused with the line
	//  number that is user-authored
	size_t mTrueLineNumber = 0;

	// The type, which determines how fields are interpreted
	RawEntryType mEntryType = RawEntryType::Invalid;

	// Alias - Type
	AliasType mAliasType = AliasType::Invalid;

	// Speech - LocID
	LocIDLineNum mLocIDLineNumber = kInvalidLocIDLineNum;

	// Alias - Shorthand
	// Layout - LayoutID
	// Command - CommandID
	// Scene - SceneID
	// Speech - CharacterID
	// Label - Label
	// Condition - Target
	rftl::string_view mPrimary;

	// Alias - Replacement value
	// Speech - ExpresionID
	// Condition - Condition key
	rftl::string_view mSecondary;

	// Speech - Dialogue text
	// Condition - Condition value
	rftl::string_view mTertiary;

	// Command - Params
	// Scene - Params
	KVPairs mKVPairs;
};

///////////////////////////////////////////////////////////////////////////////
}
