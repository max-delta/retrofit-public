#include "stdafx.h"

#include "core_thread/AsyncThread.h"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

TEST( AsyncThread, Basic )
{
	AsyncThread thread;
	ASSERT_FALSE( thread.IsStarted() );

	rftl::atomic<bool> prepped = false;
	auto prep = [&prepped]() -> void //
	{
		prepped.store( true, rftl::memory_order::memory_order_release );
	};

	rftl::atomic<bool> ready = false;
	rftl::atomic<bool> ran = false;
	auto work = [&ran]() -> void //
	{
		ran.store( true, rftl::memory_order::memory_order_release );
	};
	auto workCheck = [&ready, &ran]() -> bool //
	{
		return //
			( ready.load( rftl::memory_order::memory_order_acquire ) == true ) &&
			( ran.load( rftl::memory_order::memory_order_acquire ) == false );
	};

	static constexpr auto pause = []() -> void //
	{
		rftl::this_thread::sleep_for( rftl::chrono::milliseconds( 10 ) );
	};

	thread.Init(
		rftl::move( prep ),
		rftl::move( work ),
		rftl::move( workCheck ),
		nullptr );
	ASSERT_FALSE( thread.IsStarted() );

	thread.Start();
	ASSERT_TRUE( thread.IsStarted() );

	pause();
	ASSERT_FALSE( ready.load( rftl::memory_order::memory_order_acquire ) );
	ASSERT_FALSE( ran.load( rftl::memory_order::memory_order_acquire ) );
	ready.store( true, rftl::memory_order::memory_order_release );

	thread.Wake();
	ASSERT_TRUE( thread.IsStarted() );

	while( ran.load( rftl::memory_order::memory_order_acquire ) == false )
	{
		pause();
	}
	ASSERT_TRUE( prepped.load( rftl::memory_order::memory_order_acquire ) );
	ASSERT_TRUE( ready.load( rftl::memory_order::memory_order_acquire ) );
	ASSERT_TRUE( ran.load( rftl::memory_order::memory_order_acquire ) );
	ASSERT_TRUE( thread.IsStarted() );

	thread.Stop();
	ASSERT_TRUE( prepped.load( rftl::memory_order::memory_order_acquire ) );
	ASSERT_TRUE( ready.load( rftl::memory_order::memory_order_acquire ) );
	ASSERT_TRUE( ran.load( rftl::memory_order::memory_order_acquire ) );
	ASSERT_FALSE( thread.IsStarted() );
}

///////////////////////////////////////////////////////////////////////////////
}
