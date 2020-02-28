#include "stdafx.h"
#include "CompanyManager.h"

#include "cc3o3/state/components/Progression.h"


namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////

CompanyManager::CompanyManager( WeakPtr<file::VFS const> const& vfs )
	: mVfs( vfs )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

CompanyManager::ElementCounts CompanyManager::CalcTotalElements( state::comp::Progression const& progression ) const
{
	// TODO
	return ElementCounts();
}

///////////////////////////////////////////////////////////////////////////////
}
