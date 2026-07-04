#include "stdafx.h"
#include "LocKey.h"

#include "core/macros.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

LocKey::LocKey( rftl::string_view id )
	: mID( id )
{
	RF_ASSERT( mID.empty() == false );
}



rftl::string LocKey::GetAsDiagnosticString() const
{
	return rftl::string( mID );
}

///////////////////////////////////////////////////////////////////////////////
}
