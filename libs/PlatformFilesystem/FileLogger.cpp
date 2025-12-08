#include "stdafx.h"
#include "FileLogger.h"

#include "PlatformFilesystem/VFS.h"

#include "Logging/StandardLoggingHelpers.h"

#include "core_thread/LockedData.h"
#include "core_vfs/SeekHandle.h"

#include "rftl/array"


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
	using namespace RF::logging;

	constexpr size_t kBufSize = 512;
	rftl::array<char8_t, kBufSize> buffer;

	LogBufferResult const result = LogToBuffer(
		rftl::byte_span( buffer.begin(), buffer.end() ),
		LogBufferOptions{},
		event,
		args );
	if( result.mNullTerminatedOutput.empty() )
	{
		return;
	}

	rftl::byte_span const withoutNull =
		result.mNullTerminatedOutput.subspan( 0, result.mNullTerminatedOutput.size() - 1 );
	seekable.WriteBytes(
		withoutNull.data(),
		withoutNull.size() );
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
