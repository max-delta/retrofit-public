#include "stdafx.h"
#include "BinDump.h"

#include "CommandLine/ArgParse.h"
#include "Logging/ANSIConsoleLogger.h"
#include "Logging/AssertLogger.h"
#include "Logging/Logging.h"

#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformUtils_win32/loggers/DebuggerLogger.h"
#include "PlatformUtils_win32/Console.h"

#include "core_coff/CoffHeader.h"
#include "core_math/math_bits.h"
#include "core_pe/DosHeader.h"
#include "core_pe/PeHeader.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

#include "rftl/extension/algorithms.h"


namespace RF::bindump {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static UniquePtr<cli::ArgParse const> sCommandLineArgs;
static UniquePtr<file::VFS> sVfs;



void EmitError( char const* str )
{
	fputs( "ERROR: ", stderr );
	fputs( str, stderr );
	fputc( '\n', stderr );
}
void EmitError( rftl::string const& str )
{
	EmitError( str.c_str() );
}



void EmitUsage()
{
	static constexpr auto emit = []( char const* str ) -> void
	{
		if( str[0] != '\0' )
		{
			fputs( str, stderr );
		}
		fputc( '\n', stderr );
	};

	emit( "Usage: bindump [OPTION]... FILE" );
	emit( "Interpret various binary files." );
	emit( "Example: bindump coff.obj" );
	emit( "Only one file can be provided." );
	emit( "Use -- to end option parsing before arguments." );
	emit( "" );
	emit( "Miscellaneous:" );
	emit( "  -h, --help   Display this help text and exit" );
	emit( "" );
	emit( "Output control:" );
	emit( "  -l, --log    Log to standard output" );
}

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

	if( details::sCommandLineArgs->HasAnyOption( { "-l", "--log" } ) )
	{
		RFLOG_MILESTONE( nullptr, RFCAT_BINDUMP, "Initializing console logging..." );
		bool const consoleInitialized = platform::console::EnableANSIEscapeSequences();
		if( consoleInitialized )
		{
			puts( " == \x1b[1;32mANSI CONSOLE SUPPORT\x1b[0m ==" );
			logging::HandlerDefinition def;
			def.mSupportedSeverities = math::GetAllBitsSet<logging::SeverityMask>();
			def.mUtf8HandlerFunc = logging::ANSIConsoleLogger;
			logging::RegisterHandler( def );
		}
		else
		{
			puts( " == NO ANSI CONSOLE SUPPORT ==" );
			RF_TODO_BREAK_MSG( "Non-ANSI console logger" );
		}
	}

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

	cli::ArgParse const& argParse = *details::sCommandLineArgs;

	// --help
	if( argParse.HasAnyOption( { "-h", "--help" } ) )
	{
		details::EmitUsage();
		return ErrorReturnCode::NonActionableArg;
	}

	// File arg
	cli::ArgParse::Arguments const args = argParse.GetArguments();
	if( args.empty() )
	{
		details::EmitError( "No arguments provided" );
		details::EmitUsage();
		return ErrorReturnCode::BadArgUsage;
	}
	if( args.size() > 1 )
	{
		details::EmitError( "Too many arguments provided" );
		details::EmitUsage();
		return ErrorReturnCode::BadArgUsage;
	}
	file::VFSPath const path =
		file::VFS::kRoot.GetChild(
			file::VFSPath::CreatePathFromString(
				rftl::replace_copy(
					args.at( 0 ),
					'\\',
					file::kPathDelimiter ) ) );
	RFLOG_MILESTONE( path, RFCAT_BINDUMP, "Artifact determined from command line" );

	// Open the file for read
	file::FileHandlePtr const filePtr =
		details::sVfs->GetFileForRead(
			path );
	if( filePtr == nullptr )
	{
		details::EmitError( "Failed to open file" );
		details::EmitError( path.CreateString() );
		return ErrorReturnCode::FileAccessError;
	}

	// Get a seekable for the file
	WeakPtr<rftl::streambuf> const seekableHandle = filePtr->GetUnsafeTransientStreamBuffer();
	RF_ASSERT( seekableHandle != nullptr );
	rftl::streambuf& seekable = *seekableHandle;

	// Is it a PE file?
	{
		bin::pe::DosHeader dos = {};
		bool const isDos = dos.TryRead( seekable, 0 );
		if( isDos )
		{
			RFLOG_INFO( path, RFCAT_BINDUMP, "Looks like a DOS file" );

			bin::pe::PeHeader pe = {};
			bool const isPE = pe.TryRead( seekable, dos.mAbsoluteOffsetToPEHeader );
			if( isPE )
			{
				RFLOG_INFO( path, RFCAT_BINDUMP, "Looks like a PE file" );

				bin::coff::CoffHeader coff = {};
				bool const isCoff = coff.TryRead( seekable, dos.mAbsoluteOffsetToPEHeader + pe.mRelativeOffsetToCOFFHeader );
				if( isCoff )
				{
					RFLOG_INFO( path, RFCAT_BINDUMP, "Looks like a COFF file" );
				}
			}
		}
	}

	// TODO
	return ErrorReturnCode::Success;
}



int Shutdown( ErrorReturnCode code )
{
	int const retVal = math::enum_bitcast( code );
	if( retVal == 0 )
	{
		RFLOG_MILESTONE( nullptr, RFCAT_BINDUMP, "Shutting down with return code %i", retVal );
	}
	else
	{
		RFLOG_ERROR( nullptr, RFCAT_BINDUMP, "Shutting down with return code %i", retVal );
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
