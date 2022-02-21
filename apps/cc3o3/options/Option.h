#pragma once
#include "cc3o3/options/OptionDesc.h"

#include "rftl/string_view"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

struct Option
{
	static Option MakeAction(
		rftl::string_view identifier,
		rftl::string_view displayName,
		OptionDesc::Action::Func&& func );

	struct ListItemInitializer
	{
		rftl::string_view mIdentifier;
		rftl::string_view mDisplayName;
	};
	static Option MakeList(
		rftl::string_view identifier,
		rftl::string_view displayName,
		rftl::initializer_list<ListItemInitializer> list );

	using Identifier = rftl::string;
	Identifier mIdentifier;
	rftl::string mDisplayName;
	OptionDesc mDesc;
};

///////////////////////////////////////////////////////////////////////////////
}
