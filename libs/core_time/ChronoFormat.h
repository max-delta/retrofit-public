#pragma once
#include "rftl/chrono"
#include "rftl/string"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////

// Four digits is a decent way to format time, as it's few enough digits to be
//  readable well, and a rollover to the next size still has atleast two digits
//  of precision (9999ns + 1ns => 10us)
// Also, this uses more readable units:
//  * ns => nanos
//  * us => micros
//  * ms => millis
//  * s => sec
//  * m => min
//  * h => hrs
//  * d => days
rftl::string FormatChronoToFourDigits( rftl::chrono::nanoseconds dur );
rftl::string FormatChronoToFourDigits( rftl::chrono::microseconds dur );
rftl::string FormatChronoToFourDigits( rftl::chrono::milliseconds dur );
rftl::string FormatChronoToFourDigits( rftl::chrono::seconds dur );
rftl::string FormatChronoToFourDigits( rftl::chrono::minutes dur );
rftl::string FormatChronoToFourDigits( rftl::chrono::hours dur );

// For more exotic types, conversion to nanoseconds happens before going
//  through the rest of the normal chain
template<typename ChronoT>
rftl::string FormatChronoToFourDigits( ChronoT dur )
{
	using namespace rftl::chrono;
	static_assert( ChronoT::period::num == 1,
		"Overly exotic chrono type" );
	static_assert( ChronoT::period::den <= nanoseconds::period::den,
		"Overly exotic chrono type" );
	return FormatChronoToFourDigits( duration_cast<nanoseconds>( dur ) );
}

///////////////////////////////////////////////////////////////////////////////
}
