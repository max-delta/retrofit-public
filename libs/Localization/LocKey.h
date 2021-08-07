#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocKey
{
	//
	// Public methods
public:
	explicit LocKey( rftl::string const& id );
	explicit LocKey( rftl::string && id );

	rftl::string const& GetAsString() const;


	//
	// Private data
private:
	rftl::string mID;
};

///////////////////////////////////////////////////////////////////////////////
}
