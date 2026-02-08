#include "stdafx.h"

#include "core_time/ChronoFormat.h"

#include "rftl/ratio"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////

TEST( ChronoFormat, Basic )
{
	using namespace rftl::literals::chrono_literals;

	static constexpr auto fmt = []( auto dur ) -> rftl::string
	{
		return FormatChronoToFourDigits( dur );
	};

	// Ones
	ASSERT_EQ( fmt( 1ns ), "1nanos" );
	ASSERT_EQ( fmt( 1us ), "1micros" );
	ASSERT_EQ( fmt( 1ms ), "1millis" );
	ASSERT_EQ( fmt( 1s ), "1sec" );
	ASSERT_EQ( fmt( 1min ), "1min" );
	ASSERT_EQ( fmt( 1h ), "1hrs" );

	// Max
	ASSERT_EQ( fmt( 9'999ns ), "9999nanos" );
	ASSERT_EQ( fmt( 9'999us ), "9999micros" );
	ASSERT_EQ( fmt( 9'999ms ), "9999millis" );
	ASSERT_EQ( fmt( 9'999s ), "9999sec" );
	ASSERT_EQ( fmt( 9'999min ), "9999min" );
	ASSERT_EQ( fmt( 9'999h ), "9999hrs" );

	// Rollover
	ASSERT_EQ( fmt( 10'000ns ), "10micros" );
	ASSERT_EQ( fmt( 10'000us ), "10millis" );
	ASSERT_EQ( fmt( 10'000ms ), "10sec" );
	ASSERT_EQ( fmt( 10'000s ), "166min" );
	ASSERT_EQ( fmt( 10'000min ), "166hrs" );
	ASSERT_EQ( fmt( 10'000h ), "416days" );

	// Multiple rollover
	ASSERT_EQ( fmt( 10'000'000ns ), "10millis" );
	ASSERT_EQ( fmt( 10'000'000us ), "10sec" );
	ASSERT_EQ( fmt( 10'000'000ms ), "166min" );
	ASSERT_EQ( fmt( 10'000'000s ), "2777hrs" );
	ASSERT_EQ( fmt( 10'000'000min ), "6944days" );

	{
		using namespace rftl::chrono;
		using rftl::ratio;
		using InterruptTime =
			duration<
				nanoseconds::rep,
				ratio<1, nanoseconds::period::den / 100>>;
		ASSERT_EQ( fmt( InterruptTime( 1 ) ), "100nanos" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
