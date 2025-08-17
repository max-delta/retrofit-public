#include "stdafx.h"
#include "AssertLogger.h"

#include "Logging/Logging.h"

#include "core_math/Hash.h"

#include "rftl/extension/bounded_overwrite_iterator.h"
#include "rftl/unordered_set"
#include "rftl/string"
#include "rftl/mutex"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

void AssertLogger( LoggingRouter const& router, LogEvent<char8_t> const& event, rftl::format_args&& args )
{
	// Multiple threads could assert simultaneously, but we don't really care
	//  about the performance impact in that case, so we'll just lock the
	//  entire handler
	static rftl::mutex mutex;
	rftl::unique_lock<rftl::mutex> lock{ mutex };

	using AssertLocation = rftl::pair<rftl::string, size_t>;
	using AssertLocationHash = math::PairHash<
		AssertLocation::first_type,
		AssertLocation::second_type,
		rftl::hash<AssertLocation::first_type>,
		rftl::hash<AssertLocation::second_type>>;
	using AssertLocations = rftl::unordered_set<AssertLocation, AssertLocationHash>;
	static AssertLocations skippedAssertLocations;

	AssertLocation const thisLocation{ event.mTransientFileString, event.mLineNumber };
	if( skippedAssertLocations.count( thisLocation ) == 0 )
	{
		// C++20 APIs won't take Unicode, hope that ASCII is good enough
		rftl::string_view const legacyFormatString = rftl::string_view( reinterpret_cast<char const*>( event.mTransientMessageFormatString.data() ), event.mTransientMessageFormatString.size() );

		constexpr size_t kBufSize = 512;
		rftl::array<char, kBufSize> messageBuffer;
		{
			rftl::bounded_forward_overwrite_iterator out( messageBuffer );
			out = rftl::vformat_to( out, legacyFormatString, args );
			*out = '\0';
			*messageBuffer.rbegin() = '\0';
		}

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
}
