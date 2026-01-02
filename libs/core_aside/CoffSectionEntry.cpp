#include "stdafx.h"
#include "CoffSectionEntry.h"

#include "core/macros.h"


namespace RF::aside {
///////////////////////////////////////////////////////////////////////////////

static_assert(
	CoffSectionEntry( "Endian", 0x1122334455667788ull ).AsUInt64() ==
	0x1122334455667788ull );

///////////////////////////////////////////////////////////////////////////////

bool CoffSectionEntry::OverwriteEntry( rftl::span<CoffSectionEntry> entries, CoffSectionEntry const& overwrite )
{
	bool hasWritten = false;
	for( CoffSectionEntry& entry : entries )
	{
		if( entry.mName == overwrite.mName )
		{
			RF_ASSERT( hasWritten == false );
			entry.mData = overwrite.mData;
			hasWritten = true;
		}
	}
	return hasWritten;
}

///////////////////////////////////////////////////////////////////////////////
}
