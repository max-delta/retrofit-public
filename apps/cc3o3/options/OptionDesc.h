#pragma once
#include "cc3o3/options/OptionsFwd.h"

#include "rftl/functional"
#include "rftl/optional"
#include "rftl/string"
#include "rftl/string_view"
#include "rftl/vector"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

struct OptionDesc
{
	// Action - Do something
	struct Action
	{
		using FuncSig = void();
		using Func = rftl::function<FuncSig>;
		Func mFunc;
		bool mEnabledByDefault = false;
	};
	rftl::optional<Action> mAction;

	// List - Choose something
	struct List
	{
		struct Item
		{
			using Identifier = rftl::string;
			Identifier mIdentifier;
			rftl::string mDisplayName;
		};
		using Items = rftl::vector<Item>;
		Items mItems;
		size_t mDefault = 0;

		bool FindIdentifier( size_t& index, rftl::string_view identifier ) const;
	};
	rftl::optional<List> mList;
};

///////////////////////////////////////////////////////////////////////////////
}
