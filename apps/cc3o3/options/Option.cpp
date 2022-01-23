#include "stdafx.h"
#include "Option.h"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

Option Option::MakeAction( rftl::string_view name, OptionValue::Action::Func&& func )
{
	Option retVal = {};
	retVal.mName = name;
	retVal.mValue.mAction.emplace();

	retVal.mValue.mAction->mFunc = rftl::move( func );

	return retVal;
}



Option Option::MakeList( rftl::string_view name, rftl::initializer_list<rftl::string_view> list )
{
	Option retVal = {};
	retVal.mName = name;
	retVal.mValue.mList.emplace();

	OptionValue::List::Items& items = retVal.mValue.mList->items;
	for( rftl::string_view const& entry : list )
	{
		items.emplace_back( entry );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
