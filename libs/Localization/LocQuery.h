#pragma once
#include "Localization/LocKey.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocQuery
{
	//
	// Public methods
public:
	explicit LocQuery( LocKey const& key );
	explicit LocQuery( LocKey && key );


	//
	// Public data
public:
	LocKey mKey;
};

///////////////////////////////////////////////////////////////////////////////
}
