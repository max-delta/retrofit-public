#include "stdafx.h"
#include "ProcessLaunch.h"

#include "Logging/Logging.h"

#include "core_platform/inc/windows_inc.h"
#include "core_platform/uuid.h"

#include "rftl/string"


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
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to create job object: ERR %i", win32::GetLastError() );
		return nullptr;
	}

	win32::DWORD const lastSuccessErr = win32::GetLastError();
	if( lastSuccessErr != ERROR_SUCCESS )
	{
		RF_ASSERT( lastSuccessErr == ERROR_ALREADY_EXISTS );
		RFLOG_WARNING( nullptr, RFCAT_PLATFORMUTILS, "Partial error on job object, assumed to be a re-use", lastSuccessErr );
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
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to query job object: ERR %i", win32::GetLastError() );
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
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to update job object: ERR %i", win32::GetLastError() );
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
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to assign job object: ERR %i", win32::GetLastError() );
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
	RFLOG_MILESTONE( nullptr, RFCAT_PLATFORMUTILS, u"Launching process: %ls", mutableCommandLine.c_str() );
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
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to launch process: ERR %i", win32::GetLastError() );
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
		RFLOG_ERROR( nullptr, RFCAT_PLATFORMUTILS, "Failed to resume suspended process: ERR %i", win32::GetLastError() );

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
			RFLOG_NOTIFY( nullptr, RFCAT_PLATFORMUTILS, "Failed to terminate failed process: ERR %i", win32::GetLastError() );
		}
	}
	win32::CloseHandle( processInfo.hProcess );
	win32::CloseHandle( processInfo.hThread );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
