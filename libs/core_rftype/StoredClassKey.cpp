#include "stdafx.h"
#include "StoredClassKey.h"

#include "core/rf_assert.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

StoredClassKey::StoredClassKey() = default;



StoredClassKey::StoredClassKey( rftl::string&& name, math::HashVal64 hash )
	: mName( rftl::move( name ) )
	, mHash( hash )
{
	//
}



StoredClassKey::StoredClassKey( rftl::string_view name, math::HashVal64 hash )
	: mName( name )
	, mHash( hash )
{
	//
}



bool StoredClassKey::IsValid() const
{
	return mHash.has_value();
}



math::HashVal64 StoredClassKey::GetHash() const
{
	RF_ASSERT( mHash.has_value() );
	return mHash.value();
}

///////////////////////////////////////////////////////////////////////////////
}
