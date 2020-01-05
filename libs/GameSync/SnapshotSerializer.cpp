#include "stdafx.h"
#include "SnapshotSerializer.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"

#include "core_state/StateBag.h"

#include "Logging/Logging.h"

#include "rftl/deque"


namespace RF { namespace sync {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename ValueT>
using StateTree = rollback::Snapshot::second_type::Tree<ValueT>;
using ContainedTypes = rollback::Snapshot::second_type::ContainedTypes;



template<typename ValueT>
void AppendDiagnosticsFromTree( rftl::deque<rftl::string>& output, StateTree<ValueT> const& tree )
{
	rftl::vector<state::VariableIdentifier> const identifiers = tree.GetIdentifiers();
	for( state::VariableIdentifier const& identifier : identifiers )
	{
		WeakPtr<typename StateTree<ValueT>::StreamType const> const streamRef = tree.GetStream( identifier );
		RF_ASSERT( streamRef != nullptr );
		typename StateTree<ValueT>::StreamType const& stream = *streamRef;
		ValueT const value = stream.Read( stream.GetLatestTime() );

		rftl::stringstream ss;
		ss << '[' << id::CreateStringFromIdentifer<char, ':'>( identifier ) << ']';
		ss << " = ";
		if constexpr( rftl::is_same<uint8_t, ValueT>::value )
		{
			ss << static_cast<uint16_t>( value );
		}
		else
		{
			ss << value;
		}
		ss << ";\n";

		output.emplace_back( ss.str() );
	}
}

}
///////////////////////////////////////////////////////////////////////////////

bool SnapshotSerializer::SerializeToDiagnosticFile( rollback::Snapshot const& snapshot, file::VFS const& vfs, file::VFSPath const& filename )
{
	file::FileHandlePtr const fileHandle = vfs.GetFileForWrite( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( filename, RFCAT_GAMESYNC, "Failed to open diagnostic file for write" );
		return false;
	}

	FILE* const file = fileHandle->GetFile();
	if( file == nullptr )
	{
		RFLOG_ERROR( filename, RFCAT_GAMESYNC, "Diagnostic file handle is invalid" );
		return false;
	}

	rftl::deque<rftl::string> output;

	static_assert( rollback::Snapshot::second_type::ContainedTypes::kNumTypes == 9, "Unexpected size" );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<0>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<1>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<2>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<3>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<4>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<5>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<6>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<7>::type>() );
	details::AppendDiagnosticsFromTree( output, snapshot.second.GetTree<details::ContainedTypes::ByIndex<8>::type>() );

	// Sort for readability
	rftl::sort( output.begin(), output.end() );

	// Write to disk
	for( rftl::string const& line : output )
	{
		int const resultCode = fputs( line.c_str(), file );
		if( resultCode < 0 )
		{
			RFLOG_ERROR( filename, RFCAT_GAMESYNC, "Unknown error when writing to file" );
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}
