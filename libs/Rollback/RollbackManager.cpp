#include "stdafx.h"
#include "RollbackManager.h"

#include "Allocation/AccessorDeclaration.h"
#include "Logging/Logging.h"


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



void RollbackManager::TakeAutoSnapshot( time::CommonClock::time_point time )
{
	GetMutableSharedDomain().TakeAutoSnapshot( time );
	GetMutablePrivateDomain().TakeAutoSnapshot( time );
	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Saved auto snapshot" );
}



time::CommonClock::time_point RollbackManager::LoadLatestAutoSnapshot()
{
	Domain& share = GetMutableSharedDomain();
	Domain& priv = GetMutablePrivateDomain();

	time::CommonClock::time_point const sharedTime = share.LoadSnapshot( share.GetLatestAutoSnapshot() );
	time::CommonClock::time_point const privateTime = priv.LoadSnapshot( priv.GetLatestAutoSnapshot() );
	RFLOG_TEST_AND_FATAL( sharedTime == privateTime, nullptr, RFCAT_ROLLBACK, "Failed to load consistent auto snapshot" );

	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Loaded auto snapshot" );
	return sharedTime;
}



void RollbackManager::TakeManualSnapshot( ManualSnapshotIdentifier const& identifier, time::CommonClock::time_point time )
{
	GetMutableSharedDomain().TakeManualSnapshot( identifier, time );
	GetMutablePrivateDomain().TakeManualSnapshot( identifier, time );
	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Saved manual snapshot \"%s\"", identifier.c_str() );
}



time::CommonClock::time_point RollbackManager::LoadManualSnapshot( ManualSnapshotIdentifier const& identifier )
{
	Domain& share = GetMutableSharedDomain();
	Domain& priv = GetMutablePrivateDomain();

	WeakPtr<Snapshot const> sharedSnap = share.GetManualSnapshot( identifier );
	RFLOG_TEST_AND_FATAL( sharedSnap != nullptr, nullptr, RFCAT_ROLLBACK, "Failed to shared snapshot" );
	WeakPtr<Snapshot const> privateSnap = priv.GetManualSnapshot( identifier );
	RFLOG_TEST_AND_FATAL( privateSnap != nullptr, nullptr, RFCAT_ROLLBACK, "Failed to private snapshot" );

	time::CommonClock::time_point const sharedTime = share.LoadSnapshot( *sharedSnap );
	time::CommonClock::time_point const privateTime = priv.LoadSnapshot( *privateSnap );
	RFLOG_TEST_AND_FATAL( sharedTime == privateTime, nullptr, RFCAT_ROLLBACK, "Failed to load consistent manual snapshot" );

	RFLOG_INFO( nullptr, RFCAT_ROLLBACK, "Loaded manual snapshot \"%s\"", identifier.c_str() );
	return sharedTime;
}



time::CommonClock::time_point RollbackManager::GetHeadClock() const
{
	return mHeadClock;
}



void RollbackManager::SetHeadClock( time::CommonClock::time_point time )
{
	mHeadClock = time;
}



time::CommonClock::time_point RollbackManager::GetWindowClock() const
{
	return mWindowClock;
}



void RollbackManager::SetWindowClock( time::CommonClock::time_point time )
{
	mWindowClock = time;
}



time::CommonClock::time_point RollbackManager::GetTailClock() const
{
	return mTailClock;
}



void RollbackManager::SetTailClock( time::CommonClock::time_point time )
{
	mTailClock = time;
}

///////////////////////////////////////////////////////////////////////////////
}}
