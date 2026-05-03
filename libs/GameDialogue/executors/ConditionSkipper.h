#pragma once
#include "project.h"

#include "GameDialogue/DialogueFwd.h"

#include "core/macros.h"

#include "rftl/functional"
#include "rftl/optional"
#include "rftl/string_view"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

// Skips over entries that don't meet the needed conditions, based on the
//  key-value pairs
class GAMEDIALOGUE_API ConditionSkipper final
{
	RF_NO_INSTANCE( ConditionSkipper );

	//
	// Types and constants
public:
	using KVLookupSig = rftl::optional<rftl::string_view>( rftl::string_view key );
	using KVLookupFunc = rftl::function<KVLookupSig>;


	//
	// Public methods
public:
	static void SkipUnmetConditions( size_t& currentIndex, DialogueSequence const& sequence, const KVLookupFunc& kvLookup );
};

///////////////////////////////////////////////////////////////////////////////
}
