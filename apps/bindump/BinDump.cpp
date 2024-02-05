#include "stdafx.h"
#include "BinDump.h"

#include "CommandLine/ArgParse.h"
#include "Logging/Logging.h"
#include "Logging/AssertLogger.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformUtils_win32/loggers/DebuggerLogger.h"

#include "core_math/math_bits.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF::bindump {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static UniquePtr<cli::ArgParse const> sCommandLineArgs;
static UniquePtr<file::VFS> sVfs;

}
///////////////////////////////////////////////////////////////////////////////

ErrorReturnCode Init( cli::ArgView const& args )
{
	// Need to set up a minimum logger before logging anything, so that it
	//  doesn't use the fallback logger and pollute the standard output pipes
	{
		logging::HandlerDefinition def;
		def.mSupportedSeverities = math::GetAllBitsSet<logging::SeverityMask>();
		def.mUtf8HandlerFunc = platform::debugging::DebuggerLogger;
		logging::RegisterHandler( def );
	}
	RFLOG_MILESTONE( nullptr, RFCAT_BINDUMP, "Initializing bin dump..." );

	RFLOG_MILESTONE( nullptr, RFCAT_BINDUMP, "Tokenizing command line args..." );
	details::sCommandLineArgs = DefaultCreator<cli::ArgParse>::Create( args );

	RFLOG_MILESTONE( nullptr, RFCAT_BINDUMP, "Initializing assert logging..." );
	{
		logging::HandlerDefinition def;
		def.mSupportedSeverities = logging::Severity::RF_SEV_USER_ATTENTION_REQUESTED;
		def.mUtf8HandlerFunc = logging::AssertLogger;
		logging::RegisterHandler( def );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_BINDUMP, "Initializing VFS..." );
	details::sVfs = DefaultCreator<file::VFS>::Create();
	bool vfsInitialized = details::sVfs->AttemptPassthroughMount();
	if( vfsInitialized == false )
	{
		RFLOG_FATAL( nullptr, RFCAT_BINDUMP, "Failed to startup VFS" );
	}

	return ErrorReturnCode::Success;
}



ErrorReturnCode Process()
{
	RFLOG_MILESTONE( nullptr, RFCAT_BINDUMP, "Processing bin dump..." );

	// HACK: Try to open a file, just to show we can do so via the passthrough
	file::VFSPath const path =
		file::VFS::kRoot.GetChild(
			file::VFSPath::CreatePathFromString(
				"main.cpp" ) );
	file::FileHandlePtr const filePtr =
		details::sVfs->GetFileForRead(
			path );
	RF_ASSERT( filePtr != nullptr );

	// TODO
	return ErrorReturnCode::Success;
}

///////////////////////////////////////////////////////////////////////////////
}
