#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"
#include "rftl/string_view"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocKey
{
	//
	// Public methods
public:
	explicit LocKey( rftl::string_view const& id );
	explicit LocKey( rftl::string&& id );

	rftl::string const& GetAsString() const;


	//
	// Private data
private:
	rftl::string mID;
};

///////////////////////////////////////////////////////////////////////////////
}
