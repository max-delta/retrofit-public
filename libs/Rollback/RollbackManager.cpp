#include "stdafx.h"
#include "RollbackManager.h"

#include "Allocation/AccessorDeclaration.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

RollbackManager::RollbackManager()
	: mSharedDomain( *alloc::GetAllocator<RFTAG_ROLLBACK_SHARED_STATE>(), *alloc::GetAllocator<RFTAG_ROLLBACK_SNAPSHOTS>() )
	, mPrivateDomain( *alloc::GetAllocator<RFTAG_ROLLBACK_PRIVATE_STATE>(), *alloc::GetAllocator<RFTAG_ROLLBACK_SNAPSHOTS>() )
{
	//
}



Domain const& RollbackManager::GetSharedDomain() const
{
	return mSharedDomain;
}



Domain& RollbackManager::GetMutableSharedDomain()
{
	return mSharedDomain;
}



Domain const& RollbackManager::GetPrivateDomain() const
{
	return mPrivateDomain;
}



Domain& RollbackManager::GetMutablePrivateDomain()
{
	return mPrivateDomain;
}

///////////////////////////////////////////////////////////////////////////////
}}
