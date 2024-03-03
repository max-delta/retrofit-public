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
#include "core_coff/OptionalHeaderCommon.h"
#include "core_coff/OptionalHeaderWindows.h"
#include "core_coff/SectionHeader.h"
#include "core_math/math_bits.h"
#include "core_pe/DataDirectoryHeader.h"
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



bool TryAsPE( file::VFSPath const& logContext, rftl::streambuf& seekable )
{
	// DOS
	bin::pe::DosHeader dos = {};
	bool const isDos = dos.TryRead(
		seekable,
		0 );
	if( isDos == false )
	{
		RFLOG_INFO( logContext, RFCAT_BINDUMP, "Doesn't look like a DOS file" );
		return false;
	}
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Looks like a DOS file" );

	// PE
	bin::pe::PeHeader pe = {};
	bool const isPE = pe.TryRead(
		seekable,
		dos.mAbsoluteOffsetToPEHeader );
	if( isPE == false )
	{
		RFLOG_INFO( logContext, RFCAT_BINDUMP, "Doesn't look like a PE file" );
		return false;
	}
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Looks like a PE file" );

	// COFF
	bin::coff::CoffHeader coff = {};
	bool const isCoff = coff.TryRead(
		seekable,
		dos.mAbsoluteOffsetToPEHeader +
			pe.mRelativeOffsetToCOFFHeader );
	if( isCoff == false )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Expected a COFF header" );
		return false;
	}
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Looks like a COFF header" );

	// Optional
	if( coff.mOptionalHeaderBytes <= 0 )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Expected an optional header" );
		return false;
	}

	// Optional (common)
	bin::coff::OptionalHeaderCommon optCom = {};
	bool const hasOptCom = optCom.TryRead(
		seekable,
		dos.mAbsoluteOffsetToPEHeader +
			pe.mRelativeOffsetToCOFFHeader +
			coff.mRelativeOffsetToOptionalHeader,
		coff.mOptionalHeaderBytes );
	if( hasOptCom == false )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Expected a common optional header" );
		return false;
	}
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Looks like a common optional COFF header" );

	// Optional (Windows)
	bin::coff::OptionalHeaderWindows optWin = {};
	bool const hasOptWin = optWin.TryRead(
		seekable,
		dos.mAbsoluteOffsetToPEHeader +
			pe.mRelativeOffsetToCOFFHeader +
			coff.mRelativeOffsetToOptionalHeader +
			optCom.mRelativeOffsetToPlatformHeader,
		optCom.mHeaderType,
		coff.mOptionalHeaderBytes -
			optCom.mRelativeOffsetToPlatformHeader );
	if( hasOptWin == false )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Expected a Windows optional header" );
		return false;
	}
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Looks like a Windows optional COFF header" );

	// Data directory
	bin::pe::DataDirectoryHeader dataDir = {};
	bool const hasDataDir = dataDir.TryRead(
		seekable,
		dos.mAbsoluteOffsetToPEHeader +
			pe.mRelativeOffsetToCOFFHeader +
			coff.mRelativeOffsetToOptionalHeader +
			optCom.mRelativeOffsetToPlatformHeader +
			optWin.mRelativeOffsetToDataDirectory,
		optWin.mNumDataDirectoryEntries,
		coff.mOptionalHeaderBytes -
			optWin.mRelativeOffsetToDataDirectory );
	if( hasDataDir == false )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Expected a data directory header" );
		return false;
	}
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Looks like a data directory header" );

	// Sections
	rftl::vector<bin::coff::SectionHeader> sections = {};
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Expecting %zu sections", coff.mNumSections );
	if( coff.mNumSections > 100 )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Unreasonable number of sections" );
		return false;
	}
	sections.resize( coff.mNumSections );
	{
		size_t sectionSeekBase =
			dos.mAbsoluteOffsetToPEHeader +
			pe.mRelativeOffsetToCOFFHeader +
			coff.mRelativeOffsetToFirstSectionHeader;
		for( bin::coff::SectionHeader& section : sections )
		{
			bool const hasSection = section.TryRead( seekable, sectionSeekBase );
			if( hasSection == false )
			{
				RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Expected a section header" );
				return false;
			}
			sectionSeekBase += section.mRelativeOffsetToNextSection;
			RFLOG_DEBUG( logContext, RFCAT_BINDUMP, "Looks like a section header" );
		}
	}

	return true;
}



bool TryAsCOFF( file::VFSPath const& logContext, rftl::streambuf& seekable )
{
	// COFF
	bin::coff::CoffHeader coff = {};
	bool const isCoff = coff.TryRead(
		seekable,
		0 );
	if( isCoff == false )
	{
		RFLOG_INFO( logContext, RFCAT_BINDUMP, "Doesn't look like a COFF header" );
		return false;
	}
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Looks like a COFF header" );

	// Optional
	if( coff.mOptionalHeaderBytes != 0 )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Unexpected optional header" );
		return false;
	}

	// Sections
	rftl::vector<bin::coff::SectionHeader> sections = {};
	RFLOG_INFO( logContext, RFCAT_BINDUMP, "Expecting %zu sections", coff.mNumSections );
	if( coff.mNumSections > 10'000 )
	{
		RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Unreasonable number of sections" );
		return false;
	}
	sections.resize( coff.mNumSections );
	{
		size_t sectionSeekBase =
			coff.mRelativeOffsetToFirstSectionHeader;
		for( bin::coff::SectionHeader& section : sections )
		{
			bool const hasSection = section.TryRead( seekable, sectionSeekBase );
			if( hasSection == false )
			{
				RFLOG_ERROR( logContext, RFCAT_BINDUMP, "Expected a section header" );
				return false;
			}
			sectionSeekBase += section.mRelativeOffsetToNextSection;
			RFLOG_DEBUG( logContext, RFCAT_BINDUMP, "Looks like a section header" );
		}
	}

	return true;
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
	bool const isPE = details::TryAsPE( path, seekable );
	if( isPE )
	{
		return ErrorReturnCode::Success;
	}

	// Is it a COFF file?
	bool const isCOFF = details::TryAsCOFF( path, seekable );
	if( isCOFF )
	{
		return ErrorReturnCode::Success;
	}

	// TODO
	RFLOG_ERROR( path, RFCAT_BINDUMP, "Unable to determine file type" );
	return ErrorReturnCode::UnknownError;
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
