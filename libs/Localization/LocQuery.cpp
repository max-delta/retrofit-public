#include "stdafx.h"
#include "LocQuery.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

LocQuery::LocQuery( LocKey const& key )
	: mKey( key )
{
	//
}



LocQuery::LocQuery( LocKey&& key )
	: mKey( rftl::move( key ) )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}
