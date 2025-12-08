#include "stdafx.h"
#include "FileLogger.h"

#include "PlatformFilesystem/VFS.h"

#include "Logging/Logging.h"

#include "core_unicode/StringConvert.h"
#include "core_thread/LockedData.h"
#include "core_vfs/SeekHandle.h"

#include "rftl/extension/variadic_print.h"
#include "rftl/limits"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////
namespace details {

struct FileLogState
{
	RF_NO_COPY( FileLogState );

	FileLogState() = default;

	VFSPath mDirectory;

	SeekHandlePtr mLogFile;
};
static thread::LockedData<FileLogState> mFileLogState = {};



static void UnsetLogFileDirectory( FileLogState& state )
{
	state.mLogFile = nullptr;
	state.mDirectory = {};
}



static void SetLogFileDirectory( FileLogState& state, VFS& vfs, VFSPath const& directory )
{
	UnsetLogFileDirectory( state );

	state.mDirectory = directory;

	VFSPath const filePath = directory.GetChild( "latest.log" );
	state.mLogFile = vfs.GetFileForWrite( filePath );
	RF_ASSERT_MSG( state.mLogFile != nullptr, "Failed to start log file!" );
};



static void LogToSeekable(
	SeekHandle& seekable,
	logging::LoggingRouter const& router,
	logging::LogEvent<char8_t> const& event,
	rftl::format_args const& args )
{
	using namespace logging;

	// C++20 APIs won't take Unicode, hope that ASCII is good enough
	rftl::string_view const legacyFormatString = rftl::string_view( reinterpret_cast<char const*>( event.mTransientMessageFormatString.data() ), event.mTransientMessageFormatString.size() );

	constexpr size_t kBufSize = 512;
	rftl::array<char, kBufSize> messageBuffer;
	rftl::var_vformat_to( messageBuffer, legacyFormatString, args );
	*messageBuffer.rbegin() = '\0';

	char const* severity;
	if( event.mSeverityMask & RF_SEV_MILESTONE )
	{
		severity = "MILESTONE";
	}
	else if( event.mSeverityMask & RF_SEV_CRITICAL )
	{
		severity = "CRITICAL";
	}
	else if( event.mSeverityMask & RF_SEV_ERROR )
	{
		severity = "ERROR";
	}
	else if( event.mSeverityMask & RF_SEV_WARNING )
	{
		severity = "WARNING";
	}
	else if( event.mSeverityMask & RF_SEV_INFO )
	{
		severity = "INFO";
	}
	else if( event.mSeverityMask & RF_SEV_DEBUG )
	{
		severity = "DEBUG";
	}
	else if( event.mSeverityMask & RF_SEV_TRACE )
	{
		severity = "TRACE";
	}
	else
	{
		severity = "UNKNOWN";
	}

	rftl::array<char, kBufSize> outputBuffer;
	int bytesParsed;
	if( event.mTransientContextString.empty() )
	{
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s][%s]  %s\n", severity, event.mCategoryKey, &messageBuffer[0] );
	}
	else
	{
		rftl::string const asciiContext = unicode::ConvertToASCII( event.mTransientContextString );
		bytesParsed = snprintf( &outputBuffer[0], kBufSize, "[%s][%s]  <%s> %s\n", severity, event.mCategoryKey, asciiContext.c_str(), &messageBuffer[0] );
	}
	*outputBuffer.rbegin() = '\0';

	rftl::string_view const upToNull = outputBuffer.data();

	seekable.WriteBytes( upToNull.data(), upToNull.size() );
	static_assert( kBufSize <= static_cast<size_t>( rftl::numeric_limits<int>::max() ), "Unexpected truncation" );
	if( bytesParsed >= static_cast<int>( kBufSize ) )
	{
		static constexpr rftl::string_view kTruncated = "<TRUNCATED MESSAGE!>\n";
		seekable.WriteBytes( kTruncated.data(), kTruncated.size() );
	}
}


}
///////////////////////////////////////////////////////////////////////////////

void FileLogger( logging::LoggingRouter const& router, logging::LogEvent<char8_t> const& event, rftl::format_args const& args )
{
	details::mFileLogState.Write( [&router, &event, &args]( details::FileLogState& state ) -> void
		{
			if( state.mLogFile == nullptr )
			{
				return;
			}

			details::LogToSeekable( *state.mLogFile, router, event, args );
		} );
}



void SetLogFileDirectory( VFS& vfs, VFSPath const& directory )
{
	details::mFileLogState.Write( [&vfs, &directory]( details::FileLogState& state ) -> void
		{
			details::SetLogFileDirectory( state, vfs, directory );
		} );
}



void UnsetLogFileDirectory()
{
	details::mFileLogState.Write( []( details::FileLogState& state ) -> void
		{
			return details::UnsetLogFileDirectory( state );
		} );
}

///////////////////////////////////////////////////////////////////////////////
}
