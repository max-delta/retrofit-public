#include "stdafx.h"
#include "CompanyManager.h"

#include "cc3o3/state/components/Progression.h"
#include "cc3o3/elements/ElementDatabase.h"


namespace RF::cc::company {
///////////////////////////////////////////////////////////////////////////////

CompanyManager::CompanyManager(
	WeakPtr<file::VFS const> const& vfs,
	WeakPtr<element::ElementDatabase const> const& elementDatabase )
	: mVfs( vfs )
	, mElementDatabase( elementDatabase )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

CompanyManager::ElementCounts CompanyManager::CalcTotalElements( state::ObjectRef const& company ) const
{
	// TODO: Get progression component
	//RF_ASSERT( company.IsSet() );
	return mElementDatabase->GetElementsBasedOnTier( company::kMaxStoryTier );
}



CompanyManager::ElementCounts CompanyManager::CalcTotalElements( state::comp::Progression const& progression ) const
{
	// TODO: Additional unlocks from progression
	return mElementDatabase->GetElementsBasedOnTier( progression.mStoryTier );
}

///////////////////////////////////////////////////////////////////////////////
}
