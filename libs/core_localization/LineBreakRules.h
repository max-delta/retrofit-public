#pragma once
#include "core_localization/LocFwd.h"

#include "rftl/extension/unique_char_set.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

// Rules for how text can be split apart, particularly with respect to lines
struct LineBreakRules
{
	// These characters can have a break performed across them, the common case
	//  being whitespace and hyphens
	rftl::unique_char_set mBreakableChars;

	// These characters can be ommitted / consumed across a break, the common
	//  case being whitespace
	rftl::unique_char_set mConsumableChars;
};

///////////////////////////////////////////////////////////////////////////////
}
