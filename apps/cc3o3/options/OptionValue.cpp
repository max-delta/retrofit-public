#include "stdafx.h"
#include "OptionDesc.h"
#include "OptionValue.h"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

OptionValue OptionValue::MakeDefault( OptionDesc const& desc )
{
	OptionValue retVal = {};

	// Action
	if( desc.mAction.has_value() )
	{
		retVal.mAction.emplace();
		retVal.mAction->mEnabled = desc.mAction->mEnabledByDefault;
	}

	// List
	if( desc.mList.has_value() )
	{
		retVal.mList.emplace();
		retVal.mList->mCurrent = desc.mList->mItems.at( desc.mList->mDefault ).mIdentifier;
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
