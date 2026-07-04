#pragma once
#include "Localization/LocKey.h"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocQuery
{
	//
	// Friends
private:
	friend class LocEngine;


	//
	// Public methods
public:
	explicit LocQuery( LocKey const& key );
	explicit LocQuery( rftl::u32string_view dynamicTarget );


	//
	// Private data
private:
	LocKey mKey;
	rftl::u32string_view mDynamicTarget;
};

///////////////////////////////////////////////////////////////////////////////
}
