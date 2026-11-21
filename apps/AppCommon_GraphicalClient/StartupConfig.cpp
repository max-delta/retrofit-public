#include "stdafx.h"
#include "StartupConfig.h"

#include "CommandLine/ArgParse.h"

#include "core/rf_assert.h"


namespace RF::app {
///////////////////////////////////////////////////////////////////////////////

void StartupConfig::AdjustFromArgs( cli::ArgParse const& args )
{
	auto const boolFlag =
		[&args](
			bool& target,
			rftl::string_view on,
			rftl::string_view off ) -> void
	{
		// False overrides true if both are set
		bool hasOn = false;
		if( args.HasOption( on ) )
		{
			hasOn = true;
			target = true;
		}
		if( args.HasOption( off ) )
		{
			RF_ASSERT_MSG( hasOn == false, "Conflicting args" );
			target = false;
		}
	};

	boolFlag( mUseFrameClockForLogTime, "--logframe", "--logclock" );

	boolFlag( mInformativeLogging, "--loginfo", "--lognoinfo" );
	boolFlag( mVerboseLogging, "--logdebug", "--lognodebug" );
	boolFlag( mTraceLogging, "--logtrace", "--lognotrace" );
	boolFlag( mFileLogFlushing, "--logflush", "--lognoflush" );
}

///////////////////////////////////////////////////////////////////////////////
}
