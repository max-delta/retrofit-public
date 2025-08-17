#include "stdafx.h"
#include "ProcessLaunch.h"

#include "PlatformUtils_win32/WChar.h"

#include "Logging/Logging.h"

#include "core_platform/inc/windows_inc.h"
#include "core_platform/uuid.h"

#include "rftl/string"

RF_TODO_ANNOTATION( "Move this hack to somewhere better" );
template<>
struct rftl::formatter<rftl::wstring, char> : rftl::formatter<rftl::string_view, char>
{
	using Shim = rftl::string_view;
	using Base = rftl::formatter<Shim, char>;

	template<class ParseContext>
	constexpr typename ParseContext::iterator parse( ParseContext& ctx )
	{
		return Base::parse( ctx );
	}

	template<class FmtContext>
	typename FmtContext::iterator format( rftl::wstring const& arg, FmtContext& ctx ) const
	{
		rftl::u8string const asUTF8 = RF::platform::widechar::ConvertWideChars( arg );
		rftl::string HACK_ASCII = {};
		HACK_ASCII.reserve( asUTF8.size() );
		for( char8_t const& ch : asUTF8 )
		{
			// HACK: Drop the Unicode on the floor, since C++20 still doesn't
			//  support Unicode
			RF_CPP23_TODO( "Check to see if C++ supports Unicode yet..." )
			// SEE: https://stackoverflow.com/questions/77250832/using-stdformat-for-formatting-char8-t-char16-t-and-char32-t-texts-in-c-20
			// SEE: https://github.com/sg16-unicode/sg16/issues/68
			HACK_ASCII.push_back( static_cast<char>( ch < 127 ? ch : '?' ) );
		}
		return Base::format( static_cast<Shim>( HACK_ASCII ), ctx );
	}
};

namespace RF::platform::process {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// There seems to be no practical way to find anonymous job objects, so we'll
//  generate a name for the one our application owns
static Uuid GetJobUUID()
{
	static Uuid const sJobUUID = Uuid::GenerateNewUuid();
	return sJobUUID;
}



win32::HANDLE EnsureJobObject()
{
	// NOTE: MS IsProcessInJob(...) doesn't work, unclear why

	win32::HANDLE const job = win32::CreateJobObjectA(
		nullptr, // Cannot inherit
		GetJobUUID().GetDebugString().c_str() );
	if( job == nullptr )
	{
		RFLOGF_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to create job object: ERR {}", win32::GetLastError() );
		return nullptr;
	}

	win32::DWORD const lastSuccessErr = win32::GetLastError();
	if( lastSuccessErr != ERROR_SUCCESS )
	{
		RF_ASSERT( lastSuccessErr == ERROR_ALREADY_EXISTS );
		RFLOGF_WARNING( nullptr, RFCAT_PLATFORMUTILS, "Partial error on job object, assumed to be a re-use: ERR {}", lastSuccessErr );
	}

	return job;
}



bool MakeJobAutoClose( win32::HANDLE job )
{
	RF_ASSERT( job != nullptr );

	win32::JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo = {};
	win32::BOOL const jobQuerySuccess = win32::QueryInformationJobObject(
		job,
		win32::JobObjectExtendedLimitInformation,
		&jobInfo,
		sizeof( jobInfo ),
		nullptr ); // Don't care about return length

	// NOTE: Documentation only guarantees FALSE, not TRUE
	if( jobQuerySuccess == win32::FALSE )
	{
		RFLOGF_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to query job object: ERR {}", win32::GetLastError() );
		return false;
	}

	bool const hasKillFlag = ( jobInfo.BasicLimitInformation.LimitFlags & JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE ) != 0;
	if( hasKillFlag )
	{
		// Already set
		return true;
	}

	// Kill all processes associated with job when job closes
	jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	win32::BOOL const jobSetSuccess = win32::SetInformationJobObject(
		job,
		win32::JobObjectExtendedLimitInformation,
		&jobInfo,
		sizeof( jobInfo ) );

	// NOTE: Documentation only guarantees FALSE, not TRUE
	if( jobSetSuccess == win32::FALSE )
	{
		RFLOGF_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to update job object: ERR {}", win32::GetLastError() );
		return false;
	}

	return true;
}



bool AddRemoteProcessToJob( win32::HANDLE job, win32::HANDLE process )
{
	RF_ASSERT( job != nullptr );
	RF_ASSERT( process != nullptr );

	win32::BOOL const assignSuccess = win32::AssignProcessToJobObject( job, process );

	// NOTE: Documentation only guarantees FALSE, not TRUE
	if( assignSuccess == win32::FALSE )
	{
		RFLOGF_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to assign job object: ERR {}", win32::GetLastError() );
		return false;
	}

	return true;
}

}
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API bool LaunchSelfClone( bool autoClose )
{
	// Set up a job to enable auto-close
	win32::HANDLE const currentJob = details::EnsureJobObject();
	if( currentJob == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_PLATFORMUTILS, "Failed to ensure job, auto-close may not work" );
	}
	else
	{
		bool const autoCloseEnabled = details::MakeJobAutoClose( currentJob );
		if( autoCloseEnabled == false )
		{
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMUTILS, "Failed to modify job, auto-close may not work" );
		}
	}

	// Use same command line
	rftl::wstring mutableCommandLine;
	{
		wchar_t const* const currentCommandLine = win32::GetCommandLineW();
		RF_ASSERT( currentCommandLine != nullptr );
		if( currentCommandLine[0] != L'"' )
		{
			// Security danger
			RFLOG_ERROR( nullptr, RFCAT_PLATFORMUTILS, "GetCommandLineW() return unescaped application name" );
			RF_DBGFAIL();
			return false;
		}

		// For some bizarre reason, CreateProcessW(...) is documented to
		//  potentially modify the buffer it's given, and is not just another
		//  failed API signature w.r.t. const
		mutableCommandLine = currentCommandLine;
	}

	win32::STARTUPINFOW startupInfo = {};
	startupInfo.cb = sizeof( startupInfo );

	win32::PROCESS_INFORMATION processInfo = {};

	// Launch process
	RFLOGF_MILESTONE( nullptr, RFCAT_PLATFORMUTILS, u8"Launching process: {}", mutableCommandLine );
	win32::BOOL const createSuccess = win32::CreateProcessW(
		nullptr, // Use command line to derive application name
		mutableCommandLine.data(), // Must actually be mutable, see docs
		nullptr, // Don't inherit handles
		nullptr, // Don't inherit handles
		win32::FALSE, // Don't inherit handles
		0 |
			CREATE_DEFAULT_ERROR_MODE | // Don't inherit error mode
			CREATE_NEW_CONSOLE | // Don't inherit console (SEE: DETACHED_PROCESS)
			CREATE_SUSPENDED, // Suspend to manipulate process handle safely
		nullptr, // Inherit environment
		nullptr, // Inherit working directory
		&startupInfo,
		&processInfo );

	// NOTE: Documentation only guarantees FALSE, not TRUE
	if( createSuccess == win32::FALSE )
	{
		RFLOGF_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to launch process: ERR {}", win32::GetLastError() );
		return false;
	}

	// Add new process to current process's job
	if( autoClose && currentJob != nullptr )
	{
		bool const addSuccess = details::AddRemoteProcessToJob( currentJob, processInfo.hProcess );
		if( addSuccess == false )
		{
			RFLOG_WARNING( nullptr, RFCAT_PLATFORMUTILS, "Failed to add new process to job, auto-close may not work" );
		}
	}

	// Resume process
	win32::DWORD const suspendCount = win32::ResumeThread( processInfo.hThread );
	if( suspendCount == static_cast<win32::DWORD>( -1 ) )
	{
		RFLOGF_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to resume suspended process: ERR {}", win32::GetLastError() );

		// Unclear how to recover, just kill it
		win32::BOOL const terminateSuccess = win32::TerminateProcess( processInfo.hProcess, 1 );

		// NOTE: Documentation only guarantees FALSE, not TRUE
		if( terminateSuccess == win32::FALSE )
		{
			// This is not good, and we may have left a zombie process on the
			//  user's machine, with no clear indication of how we managed to
			//  into such a broken state
			// TODO: Test to see if outside intervention by an external tool
			//  killing our child process can cause this behavior
			RFLOGF_NOTIFY( nullptr, RFCAT_PLATFORMUTILS, "Failed to terminate failed process: ERR {}", win32::GetLastError() );
		}
	}
	win32::CloseHandle( processInfo.hProcess );
	win32::CloseHandle( processInfo.hThread );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
