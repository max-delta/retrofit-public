#include "stdafx.h"
#include "ChronoFormat.h"

#include "core/rf_assert.h"

#include "rftl/format"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////


rftl::string FormatChronoToFourDigits( rftl::chrono::nanoseconds dur )
{
	using namespace rftl::chrono;
	using namespace rftl::literals::chrono_literals;
	if( dur <= 9999ns )
	{
		return rftl::format( "{}nanos", dur.count() );
	}
	return FormatChronoToFourDigits( duration_cast<microseconds>( dur ) );
}

rftl::string FormatChronoToFourDigits( rftl::chrono::microseconds dur )
{
	using namespace rftl::chrono;
	using namespace rftl::literals::chrono_literals;
	if( dur <= 9999us )
	{
		return rftl::format( "{}micros", dur.count() );
	}
	return FormatChronoToFourDigits( duration_cast<milliseconds>( dur ) );
}

rftl::string FormatChronoToFourDigits( rftl::chrono::milliseconds dur )
{
	using namespace rftl::chrono;
	using namespace rftl::literals::chrono_literals;
	if( dur <= 9999ms )
	{
		return rftl::format( "{}millis", dur.count() );
	}
	return FormatChronoToFourDigits( duration_cast<seconds>( dur ) );
}

rftl::string FormatChronoToFourDigits( rftl::chrono::seconds dur )
{
	using namespace rftl::chrono;
	using namespace rftl::literals::chrono_literals;
	if( dur <= 9999s )
	{
		return rftl::format( "{}sec", dur.count() );
	}
	return FormatChronoToFourDigits( duration_cast<minutes>( dur ) );
}

rftl::string FormatChronoToFourDigits( rftl::chrono::minutes dur )
{
	using namespace rftl::chrono;
	using namespace rftl::literals::chrono_literals;
	if( dur <= 9999min )
	{
		return rftl::format( "{}min", dur.count() );
	}
	return FormatChronoToFourDigits( duration_cast<hours>( dur ) );
}

rftl::string FormatChronoToFourDigits( rftl::chrono::hours dur )
{
	using namespace rftl::chrono;
	using namespace rftl::literals::chrono_literals;
	if( dur <= 9999h )
	{
		return rftl::format( "{}hrs", dur.count() );
	}
	hours::rep const days = dur / 24h;
	RF_ASSERT_MSG(
		days < 9999,
		"Why are you trying to display a time greater than 27 years?" );
	return rftl::format( "{}days", days );
}

///////////////////////////////////////////////////////////////////////////////
}
