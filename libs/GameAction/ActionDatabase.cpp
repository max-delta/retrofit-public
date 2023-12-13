#include "stdafx.h"
#include "ActionDatabase.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

ActionDatabase::ActionDatabase() = default;



bool ActionDatabase::AddAction( rftl::string&& key, UniquePtr<ActionRecord>&& action )
{
	RF_ASSERT( action != nullptr );

	if( mActionsByID.count( key ) > 0 )
	{
		// Duplicate
		return false;
	}

	bool const newEntry = mActionsByID.emplace( rftl::move( key ), rftl::move( action ) ).second;
	RF_ASSERT( newEntry );
	return true;
}



UniquePtr<ActionRecord> ActionDatabase::RemoveAction( rftl::string_view const& key )
{
	static_assert( __cplusplus < 202002L, "Replace string copy with templated find using string_view" );
	ActionsByID::iterator const iter = mActionsByID.find( rftl::string( key ) );
	if( iter == mActionsByID.end() )
	{
		// Not found
		return nullptr;
	}

	// Extract
	UniquePtr<ActionRecord> retVal = rftl::move( iter->second );
	RF_ASSERT( retVal );

	// Erase
	mActionsByID.erase( iter );

	return retVal;
}



WeakPtr<ActionRecord const> ActionDatabase::GetAction( rftl::string_view const& key ) const
{
	static_assert( __cplusplus < 202002L, "Replace string copy with templated find using string_view" );
	ActionsByID::const_iterator const iter = mActionsByID.find( rftl::string( key ) );
	if( iter == mActionsByID.end() )
	{
		// Not found
		return nullptr;
	}

	return iter->second;
}

///////////////////////////////////////////////////////////////////////////////
}
