#pragma once
#include "cc3o3/options/Option.h"
#include "cc3o3/options/OptionValue.h"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

struct OptionSet
{
	using Identifier = rftl::string;
	Identifier mIdentifier;
	rftl::string mDisplayName;

	struct Entry
	{
		Option mOption;
		OptionValue mValue;
	};
	using Options = rftl::vector<Entry>;
	Options mOptions;
};

///////////////////////////////////////////////////////////////////////////////
}
