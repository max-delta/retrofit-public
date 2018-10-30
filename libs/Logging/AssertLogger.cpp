#include "stdafx.h"
#include "AssertLogger.h"

#include "Logging/Logging.h"

#include "core_math/Hash.h"

#include "rftl/unordered_set"
#include "rftl/string"
#include "rftl/mutex"


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

void AssertLogger( LoggingRouter const& router, LogEvent<char> const& event, va_list args )
{
	// Multiple threads could assert simultaneously, but we don't really care
	//  about the performance impact in that case, so we'll just lock the
	//  entire handler
	static rftl::mutex mutex;
	rftl::unique_lock<rftl::mutex> lock{ mutex };

	using AssertLocation = rftl::pair<rftl::string, size_t>;
	using AssertLocationHash = math::PairHash<
		AssertLocation::first_type, AssertLocation::second_type,
		rftl::hash<AssertLocation::first_type>, rftl::hash<AssertLocation::second_type> >;
	using AssertLocations = rftl::unordered_set<AssertLocation, AssertLocationHash>;
	static AssertLocations skippedAssertLocations;

	AssertLocation const thisLocation{ event.mTransientFileString, event.mLineNumber };
	if( skippedAssertLocations.count( thisLocation ) == 0 )
	{
		constexpr size_t kBufSize = 512;
		rftl::array<char, kBufSize> messageBuffer;
		vsnprintf( &messageBuffer[0], kBufSize, event.mTransientMessageFormatString, args );
		*messageBuffer.rbegin() = '\0';

		assert::AssertResponse const response = assert::AssertNotification( event.mTransientFileString, event.mLineNumber, "N/A", messageBuffer.data() );
		if( response == assert::AssertResponse::Interrupt )
		{
			RF_SOFTWARE_INTERRUPT();
		}
		else if( response == assert::AssertResponse::SkipFurtherInstances )
		{
			skippedAssertLocations.emplace( thisLocation );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
