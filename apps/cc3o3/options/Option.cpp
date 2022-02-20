#include "stdafx.h"
#include "Option.h"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

Option Option::MakeAction( rftl::string_view name, OptionDesc::Action::Func&& func )
{
	Option retVal = {};
	retVal.mName = name;
	retVal.mDesc.mAction.emplace();

	retVal.mDesc.mAction->mFunc = rftl::move( func );

	return retVal;
}



Option Option::MakeList( rftl::string_view name, rftl::initializer_list<rftl::string_view> list )
{
	Option retVal = {};
	retVal.mName = name;
	retVal.mDesc.mList.emplace();

	OptionDesc::List::Items& items = retVal.mDesc.mList->items;
	for( rftl::string_view const& entry : list )
	{
		items.emplace_back( entry );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
