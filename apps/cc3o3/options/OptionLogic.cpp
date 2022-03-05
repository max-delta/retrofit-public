#include "stdafx.h"
#include "OptionLogic.h"

#include "cc3o3/options/OptionDesc.h"
#include "cc3o3/options/OptionValue.h"

#include "core_math/math_casts.h"

#include "rftl/limits"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

bool OptionLogic::FindListIdentifier( size_t& index, OptionDesc const& desc, rftl::string_view identifier )
{
	index = rftl::numeric_limits<size_t>::max();

	if( desc.mList.has_value() == false )
	{
		RF_DBGFAIL();
		return false;
	}

	OptionDesc::List const& list = *desc.mList;

	size_t const numItems = list.mItems.size();
	for( size_t i = 0; i < numItems; i++ )
	{
		OptionDesc::List::Item const& item = list.mItems.at( i );
		if( item.mIdentifier == identifier )
		{
			index = i;
			return true;
		}
	}

	return false;
}



bool OptionLogic::CycleList( OptionValue& value, OptionDesc const& desc, bool forward )
{
	if( value.mList.has_value() == false )
	{
		RF_DBGFAIL();
		return false;
	}
	if( desc.mList.has_value() == false )
	{
		RF_DBGFAIL();
		return false;
	}

	// Find the current item
	size_t index;
	bool const foundCurrent = options::OptionLogic::FindListIdentifier( index, desc, value.mList->mCurrent );
	if( foundCurrent == false )
	{
		RF_DBGFAIL();
		return false;
	}

	if( index == 0 && forward == false )
	{
		// At front
		return true;
	}

	size_t const numItems = desc.mList->mItems.size();
	if( index == numItems - 1 && forward )
	{
		// At end
		return true;
	}

	// Cycle
	index = math::integer_cast<size_t>(
		math::integer_cast<int64_t>( index ) +
		( forward ? 1 : -1 ) );

	// Update
	value.mList->mCurrent = desc.mList->mItems.at( index ).mIdentifier;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
