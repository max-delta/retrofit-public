#include "stdafx.h"

#include "Rollback/Domain.h"

#include "core_allocate/LinearAllocator.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

TEST( Domain, AutoSnapshot )
{
	alloc::AllocatorT<alloc::LinearAllocator<1024>> alloc{ ExplicitDefaultConstruct() };
	Domain domain{ alloc, alloc };

	static constexpr uint8_t kVal = 30;
	static constexpr uint8_t kResetVal = 20;
	static constexpr time::CommonClock::time_point kInvalidTime = time::CommonClock::time_point();
	static constexpr time::CommonClock::time_point kCurTime = time::CommonClock::TimePointFromNanos( 50 );
	static constexpr time::CommonClock::time_point kResetTime = time::CommonClock::TimePointFromNanos( 49 );
	state::VariableIdentifier const identifier = state::VariableIdentifier{ "Var" };

	// Initial
	{
		Snapshot const& prev = domain.GetOldestAutoSnapshot();
		ASSERT_EQ( prev.first, kInvalidTime );
		ASSERT_EQ( prev.second.GetStream<uint8_t>( identifier ), nullptr );
		Snapshot const& cur = domain.GetLatestAutoSnapshot();
		ASSERT_EQ( cur.first, kInvalidTime );
		ASSERT_EQ( cur.second.GetStream<uint8_t>( identifier ), nullptr );
	}

	// Take snapshot
	domain.TakeAutoSnapshot( kCurTime );
	{
		Snapshot const& prev = domain.GetOldestAutoSnapshot();
		ASSERT_EQ( prev.first, kInvalidTime );
		ASSERT_EQ( prev.second.GetStream<uint8_t>( identifier ), nullptr );
		Snapshot const& cur = domain.GetLatestAutoSnapshot();
		ASSERT_EQ( cur.first, kCurTime );
		ASSERT_EQ( cur.second.GetStream<uint8_t>( identifier ), nullptr );
	}

	// Write val and take snapshot
	domain.GetMutableWindow().GetOrCreateStream<uint8_t>( identifier, alloc )->Write( kCurTime, kVal );
	domain.TakeAutoSnapshot( kCurTime );
	{
		Snapshot const& prev = domain.GetOldestAutoSnapshot();
		ASSERT_EQ( prev.first, kCurTime );
		ASSERT_EQ( prev.second.GetStream<uint8_t>( identifier ), nullptr );
		Snapshot const& cur = domain.GetLatestAutoSnapshot();
		ASSERT_EQ( cur.first, kCurTime );
		ASSERT_EQ( cur.second.GetStream<uint8_t>( identifier )->Read( kCurTime ), kVal );
	}

	// Change val
	domain.GetMutableWindow().GetMutableStream<uint8_t>( identifier )->Write( kCurTime, kResetVal );
	ASSERT_EQ( domain.GetWindow().GetStream<uint8_t>( identifier )->Read( kCurTime ), kResetVal );

	// Load snapshot
	domain.LoadSnapshot( domain.GetLatestAutoSnapshot() );
	ASSERT_EQ( domain.GetWindow().GetStream<uint8_t>( identifier )->Read( kCurTime ), kVal );
}



TEST( Domain, ManualSnapshot )
{
	alloc::AllocatorT<alloc::LinearAllocator<1024>> alloc{ ExplicitDefaultConstruct() };
	Domain domain{ alloc, alloc };

	static constexpr char kSnapshot[] = "Snap";
	static constexpr uint8_t kVal = 30;
	static constexpr uint8_t kResetVal = 20;
	static constexpr time::CommonClock::time_point kInvalidTime = time::CommonClock::time_point();
	static constexpr time::CommonClock::time_point kCurTime = time::CommonClock::TimePointFromNanos( 50 );
	static constexpr time::CommonClock::time_point kResetTime = time::CommonClock::TimePointFromNanos( 49 );
	state::VariableIdentifier const identifier = state::VariableIdentifier{ "Var" };

	WeakPtr<Snapshot const> snapshot;

	// Initial
	snapshot = domain.GetManualSnapshot( kSnapshot );
	ASSERT_EQ( snapshot, nullptr );

	// Write val and take snapshot
	domain.GetMutableWindow().GetOrCreateStream<uint8_t>( identifier, alloc )->Write( kCurTime, kVal );
	snapshot = domain.TakeManualSnapshot( kSnapshot, kCurTime );
	ASSERT_NE( snapshot, nullptr );
	ASSERT_EQ( snapshot, domain.GetManualSnapshot( kSnapshot ) );
	ASSERT_EQ( snapshot->first, kCurTime );
	ASSERT_EQ( snapshot->second.GetStream<uint8_t>( identifier )->Read( kCurTime ), kVal );

	// Change val
	domain.GetMutableWindow().GetMutableStream<uint8_t>( identifier )->Write( kCurTime, kResetVal );
	ASSERT_EQ( domain.GetWindow().GetStream<uint8_t>( identifier )->Read( kCurTime ), kResetVal );

	// Load snapshot
	domain.LoadSnapshot( *snapshot );
	ASSERT_EQ( domain.GetWindow().GetStream<uint8_t>( identifier )->Read( kCurTime ), kVal );

	// Remove snapshot
	domain.RemoveManualSnapshot( kSnapshot );
	ASSERT_EQ( snapshot, nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}}
