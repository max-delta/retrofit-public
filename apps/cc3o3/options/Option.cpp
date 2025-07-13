#include "stdafx.h"
#include "Option.h"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

Option Option::MakeAction(
	rftl::string_view identifier,
	rftl::string_view displayName,
	OptionDesc::Action::Func&& func )
{
	Option retVal = {};
	retVal.mIdentifier = identifier;
	retVal.mDisplayName = displayName;
	retVal.mDesc.mAction.emplace();

	retVal.mDesc.mAction->mFunc = rftl::move( func );

	return retVal;
}



Option Option::MakeList(
	rftl::string_view identifier,
	rftl::string_view displayName,
	rftl::initializer_list<ListItemInitializer> list )
{
	Option retVal = {};
	retVal.mIdentifier = identifier;
	retVal.mDisplayName = displayName;
	retVal.mDesc.mList.emplace();

	OptionDesc::List::Items& items = retVal.mDesc.mList->mItems;
	for( ListItemInitializer const& entry : list )
	{
		OptionDesc::List::Item item = {};
		item.mIdentifier = entry.mIdentifier;
		item.mDisplayName = entry.mDisplayName;
		items.emplace_back( rftl::move( item ) );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
