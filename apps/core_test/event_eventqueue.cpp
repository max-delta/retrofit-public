#include "stdafx.h"

#include "core_event/MultithreadEventQueue.h"
#include "core_event/SinglethreadEventQueue.h"


namespace RF::event {
///////////////////////////////////////////////////////////////////////////////

TEST( EventQueue, MultithreadBasics )
{
	using Queue = MultithreadEventQueue<int>;

	Queue queue;

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 0 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 0 );
	}

	ASSERT_TRUE( queue.AddEvent( 1 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 1 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 1 );
		ASSERT_EQ( events.at(0), 1 );
	}

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 0 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 0 );
	}

	ASSERT_TRUE( queue.AddEvent( 1 ) );
	ASSERT_TRUE( queue.AddEvent( 2 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 2 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 2 );
		ASSERT_EQ( events.at( 0 ), 1 );
		ASSERT_EQ( events.at( 1 ), 2 );
	}

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 0 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 0 );
	}

	ASSERT_TRUE( queue.AddEvent( 1 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 1 );
	}

	ASSERT_TRUE( queue.AddEvent( 2 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 1 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 2 );
		ASSERT_EQ( events.at( 0 ), 1 );
		ASSERT_EQ( events.at( 1 ), 2 );
	}
}



TEST( EventQueue, SinglethreadBasics )
{
	using Queue = SinglethreadEventQueue<int>;

	Queue queue;

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 0 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 0 );
	}

	ASSERT_TRUE( queue.AddEvent( 1 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 1 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 1 );
		ASSERT_EQ( events.at( 0 ), 1 );
	}

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 0 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 0 );
	}

	ASSERT_TRUE( queue.AddEvent( 1 ) );
	ASSERT_TRUE( queue.AddEvent( 2 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 2 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 2 );
		ASSERT_EQ( events.at( 0 ), 1 );
		ASSERT_EQ( events.at( 1 ), 2 );
	}

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 0 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 0 );
	}

	ASSERT_TRUE( queue.AddEvent( 1 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 1 );
	}

	ASSERT_TRUE( queue.AddEvent( 2 ) );

	{
		size_t const numStaged = queue.StageNewEvents();
		ASSERT_EQ( numStaged, 1 );
		Queue::EventQueue const events = queue.ExtractStagedEvents();
		ASSERT_EQ( events.size(), 2 );
		ASSERT_EQ( events.at( 0 ), 1 );
		ASSERT_EQ( events.at( 1 ), 2 );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
