#pragma once
#include "cc3o3/options/OptionValue.h"

#include "rftl/string_view"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

struct Option
{
	static Option MakeAction( rftl::string_view name, OptionValue::Action::Func&& func );
	static Option MakeList( rftl::string_view name, rftl::initializer_list<rftl::string_view> list );

	rftl::string mName;
	OptionValue mValue;
};

///////////////////////////////////////////////////////////////////////////////
}
