#include "stdafx.h"
#include "LocKey.h"

#include "core/macros.h"

namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

LocKey::LocKey( rftl::string const& id )
	: mID( id )
{
	//
}



LocKey::LocKey( rftl::string&& id )
	: mID( rftl::move( id ) )
{
	//
}



rftl::string const& LocKey::GetAsString() const
{
	return mID;
}

///////////////////////////////////////////////////////////////////////////////
}}
