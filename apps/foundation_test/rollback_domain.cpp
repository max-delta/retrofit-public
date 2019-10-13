#include "stdafx.h"

#include "Rollback/Domain.h"

#include "core_allocate/LinearAllocator.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

TEST( Domain, AutoSnapshot )
{
	alloc::AllocatorT<alloc::LinearAllocator<1024>> alloc{ ExplicitDefaultConstruct() };
	Domain domain{ alloc, alloc };

	static constexpr char kName[] = "Var";
	static constexpr uint8_t kVal = 30;
	static constexpr time::CommonClock::time_point kInvalidTime = time::CommonClock::time_point();
	static constexpr time::CommonClock::time_point kCurTime = time::CommonClock::TimePointFromNanos( 50 );

	{
		Snapshot const& prev = domain.GetOldestAutoSnapshot();
		ASSERT_EQ( prev.first, kInvalidTime );
		ASSERT_EQ( prev.second.GetStream<uint8_t>( kName ), nullptr );
		Snapshot const& cur = domain.GetLatestAutoSnapshot();
		ASSERT_EQ( cur.first, kInvalidTime );
		ASSERT_EQ( cur.second.GetStream<uint8_t>( kName ), nullptr );
	}
	domain.TakeAutoSnapshot( kCurTime );
	{
		Snapshot const& prev = domain.GetOldestAutoSnapshot();
		ASSERT_EQ( prev.first, kInvalidTime );
		ASSERT_EQ( prev.second.GetStream<uint8_t>( kName ), nullptr );
		Snapshot const& cur = domain.GetLatestAutoSnapshot();
		ASSERT_EQ( cur.first, kCurTime );
		ASSERT_EQ( cur.second.GetStream<uint8_t>( kName ), nullptr );
	}
	domain.GetMutableWindow().GetOrCreateStream<uint8_t>( kName, alloc )->Write( kCurTime, kVal );
	domain.TakeAutoSnapshot( kCurTime );
	{
		Snapshot const& prev = domain.GetOldestAutoSnapshot();
		ASSERT_EQ( prev.first, kCurTime );
		ASSERT_EQ( prev.second.GetStream<uint8_t>( kName ), nullptr );
		Snapshot const& cur = domain.GetLatestAutoSnapshot();
		ASSERT_EQ( cur.first, kCurTime );
		ASSERT_EQ( cur.second.GetStream<uint8_t>( kName )->Read( kCurTime ), kVal );
	}
}



TEST( Domain, ManualSnapshot )
{
	alloc::AllocatorT<alloc::LinearAllocator<1024>> alloc{ ExplicitDefaultConstruct() };
	Domain domain{ alloc, alloc };

	static constexpr char kSnapshot[] = "Snap";
	static constexpr char kName[] = "Var";
	static constexpr uint8_t kVal = 30;
	static constexpr time::CommonClock::time_point kInvalidTime = time::CommonClock::time_point();
	static constexpr time::CommonClock::time_point kCurTime = time::CommonClock::TimePointFromNanos( 50 );

	WeakPtr<Snapshot const> snapshot;

	snapshot = domain.GetManualSnapshot( kSnapshot );
	ASSERT_EQ( snapshot, nullptr );

	domain.GetMutableWindow().GetOrCreateStream<uint8_t>( kName, alloc )->Write( kCurTime, kVal );
	snapshot = domain.TakeManualSnapshot( kSnapshot, kCurTime );
	ASSERT_NE( snapshot, nullptr );
	ASSERT_EQ( snapshot, domain.GetManualSnapshot( kSnapshot ) );
	ASSERT_EQ( snapshot->first, kCurTime );
	ASSERT_EQ( snapshot->second.GetStream<uint8_t>( kName )->Read( kCurTime ), kVal );

	domain.RemoveManualSnapshot( kSnapshot );
	ASSERT_EQ( snapshot, nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}}
