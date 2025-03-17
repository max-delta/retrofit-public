#include "stdafx.h"
#include "ConditionDatabase.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

ConditionDatabase::ConditionDatabase() = default;



bool ConditionDatabase::AddCondition( rftl::string&& key, UniquePtr<ConditionRecord>&& condition )
{
	RF_ASSERT( condition != nullptr );

	if( mConditionsByID.count( key ) > 0 )
	{
		// Duplicate
		return false;
	}

	bool const newEntry = mConditionsByID.emplace( rftl::move( key ), rftl::move( condition ) ).second;
	RF_ASSERT( newEntry );
	return true;
}



UniquePtr<ConditionRecord> ConditionDatabase::RemoveCondition( rftl::string_view const& key )
{
	ConditionsByID::iterator const iter = mConditionsByID.find( RFTL_STR_V_HASH( key ) );
	if( iter == mConditionsByID.end() )
	{
		// Not found
		return nullptr;
	}

	// Extract
	UniquePtr<ConditionRecord> retVal = rftl::move( iter->second );
	RF_ASSERT( retVal );

	// Erase
	mConditionsByID.erase( iter );

	return retVal;
}



WeakPtr<ConditionRecord> ConditionDatabase::GetCondition( rftl::string_view const& key ) const
{
	ConditionsByID::const_iterator const iter = mConditionsByID.find( RFTL_STR_V_HASH( key ) );
	if( iter == mConditionsByID.end() )
	{
		// Not found
		return nullptr;
	}

	return iter->second;
}

///////////////////////////////////////////////////////////////////////////////
}
