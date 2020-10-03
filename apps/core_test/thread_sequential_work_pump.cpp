#include "stdafx.h"

#include "core_thread/SequentialWorkPump.h"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

TEST( SequentialWorkPump, Basic )
{
	SequentialWorkPump<int> pump;
	ASSERT_FALSE( pump.IsStarted() );

	bool prepped = false;
	auto prep = [&prepped]() -> void //
	{
		prepped = true;
	};

	int accumulator = 0;
	auto exec = [&accumulator]( int&& workItem ) -> void //
	{
		accumulator += workItem;
	};

	pump.Init(
		rftl::move( prep ),
		rftl::move( exec ) );
	ASSERT_FALSE( pump.IsStarted() );

	pump.AddWorkItem( 1 );
	ASSERT_FALSE( pump.IsStarted() );

	pump.Start();
	ASSERT_TRUE( pump.IsStarted() );

	pump.AddWorkItem( 10 );
	pump.AddWorkItem( 100 );

	while( pump.HasQueuedWork() )
	{
		rftl::this_thread::sleep_for( rftl::chrono::milliseconds( 10 ) );
	}
	ASSERT_TRUE( pump.IsStarted() );

	pump.Stop();
	ASSERT_TRUE( prepped );
	ASSERT_EQ( accumulator, 111 );
	ASSERT_FALSE( pump.IsStarted() );
}

///////////////////////////////////////////////////////////////////////////////
}
