#pragma once
#include "cc3o3/options/OptionDesc.h"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

struct OptionValue
{
	// Action - Do something
	struct Action
	{
		bool mEnabled = false;
	};
	rftl::optional<Action> mAction;

	// List - Choose something
	struct List
	{
		OptionDesc::List::Item::Identifier mCurrent;
	};
	rftl::optional<List> mList;
};

///////////////////////////////////////////////////////////////////////////////
}
