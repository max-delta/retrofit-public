#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"
#include "rftl/string_view"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

// Used for key-based localization lookups
class LOCALIZATION_API LocKey
{
	//
	// Friends
private:
	friend class LocEngine;
	friend class LocQuery;


	//
	// Public methods
public:
	// NOTE: The string is not stored, so the caller must ensure the memory
	//  remains valid until all associated queries complete
	explicit LocKey( rftl::string_view id );

	rftl::string GetAsDiagnosticString() const;


	//
	// Private methods
private:
	LocKey() = default;


	//
	// Private data
private:
	rftl::string_view mID;
};

///////////////////////////////////////////////////////////////////////////////
}
