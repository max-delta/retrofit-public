#pragma once
#include "cc3o3/company/CompanyFwd.h"

#include "cc3o3/elements/ElementFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/unordered_map"


// Forwards
namespace RF::cc::state::comp {
class Progression;
}

namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////

class CompanyManager
{
	RF_NO_COPY( CompanyManager );

	//
	// Types and constants
public:
	using ElementCounts = rftl::unordered_map<element::ElementIdentifier, size_t>;


	//
	// Public methods
public:
	CompanyManager( WeakPtr<file::VFS const> const& vfs );


	//
	// Public methods
public:
	// The company's available elements are determined by progression
	ElementCounts CalcTotalElements( state::comp::Progression const& progression ) const;


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
};

///////////////////////////////////////////////////////////////////////////////
}
