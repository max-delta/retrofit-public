#pragma once
#include "cc3o3/options/OptionsFwd.h"

#include "rftl/functional"
#include "rftl/optional"
#include "rftl/string"
#include "rftl/vector"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

struct OptionValue
{
	// Action - Do something
	struct Action
	{
		using FuncSig = void();
		using Func = rftl::function<FuncSig>;
		Func mFunc;
	};
	rftl::optional<Action> mAction;

	// List - Choose something
	struct List
	{
		using Item = rftl::string;
		using Items = rftl::vector<Item>;
		Items items;
	};
	rftl::optional<List> mList;
};

///////////////////////////////////////////////////////////////////////////////
}
