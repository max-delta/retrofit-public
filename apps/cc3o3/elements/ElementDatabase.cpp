#include "stdafx.h"
#include "ElementDatabase.h"

#include "cc3o3/elements/ElementDesc.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "core_math/math_casts.h"

#include "rftl/sstream"
#include "rftl/algorithm"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

ElementDatabase::ElementDatabase( WeakPtr<file::VFS const> vfs )
	: mVfs( vfs )
{
	//
}



bool ElementDatabase::LoadDescTables( file::VFSPath const& descTablesDir )
{
	// Unload current descs
	mElementDescs.clear();

	// Enumerate desc tables
	rftl::vector<file::VFSPath> descTableFiles;
	{
		rftl::vector<file::VFSPath> unexpectedDirectories;
		mVfs->EnumerateDirectory(
			descTablesDir,
			file::VFSMount::Permissions::ReadOnly,
			descTableFiles,
			unexpectedDirectories );
		for( file::VFSPath const& unexpectedDirectory : unexpectedDirectories )
		{
			RFLOG_WARNING( unexpectedDirectory, RFCAT_CC3O3, "Unexpected directory found alongside element desc table files" );
		}
	}

	if( descTableFiles.empty() )
	{
		RFLOG_NOTIFY( descTablesDir, RFCAT_CC3O3, "Failed to find any element desc table files" );
		return false;
	}

	// Load all desc tables
	for( file::VFSPath const& descTableFile : descTableFiles )
	{
		bool const loadResult = LoadDescTable( descTableFile );
		if( loadResult == false )
		{
			RFLOG_NOTIFY( descTableFile, RFCAT_CC3O3, "Couldn't load element desc table file" );
			return false;
		}
	}

	return true;
}



ElementDesc ElementDatabase::GetElementDesc( ElementIdentifier identifier ) const
{
	ElementDesc asDesc = {};
	asDesc.mIdentifier = identifier;

	ElementDescs::const_iterator const iter = rftl::find(
		mElementDescs.begin(),
		mElementDescs.end(),
		asDesc );
	if( iter == mElementDescs.end() )
	{
		return ElementDesc{};
	}

	return *iter;
}



rftl::deque<rftl::deque<rftl::string>> ElementDatabase::LoadCSV( file::VFSPath const& path )
{
	file::FileHandlePtr const handle = mVfs->GetFileForRead( path );
	if( handle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to get file for read" );
		return {};
	}

	file::FileBuffer const buffer{ *handle.Get(), false };
	if( buffer.GetData() == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to get data from file buffer" );
		return {};
	}

	char const* const data = reinterpret_cast<char const*>( buffer.GetData() );
	size_t const size = buffer.GetSize();
	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( data, size );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to read file as csv" );
		return {};
	}

	return csv;
}



bool ElementDatabase::LoadDescTable( file::VFSPath const& descTablePath )
{
	rftl::deque<rftl::deque<rftl::string>> descTable = LoadCSV( descTablePath );
	if( descTable.empty() )
	{
		RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Failed to load element desc table file" );
		return false;
	}

	size_t line = 0;

	rftl::deque<rftl::string> const header = descTable.front();
	descTable.pop_front();
	if(
		header.size() != 5 ||
		header.at( 0 ) != "id" ||
		header.at( 1 ) != "innate" ||
		header.at( 2 ) != "base" ||
		header.at( 3 ) != "min" ||
		header.at( 4 ) != "max" )
	{
		RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc table header" );
		return false;
	}
	line++;

	while( descTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = descTable.front();
		if( entry.size() != 5 )
		{
			RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line %i, expected 5 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		rftl::string const& innate = entry.at( 1 );
		rftl::string const& baseLevel = entry.at( 2 );
		rftl::string const& minLevel = entry.at( 3 );
		rftl::string const& maxLevel = entry.at( 4 );

		ElementDesc desc = {};

		if( id.empty() || id.size() > sizeof( ElementIdentifier ) )
		{
			RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line %i, bad id", math::integer_cast<int>( line ) );
			return false;
		}
		desc.mIdentifier = MakeElementIdentifier( id );

		if( GetElementDesc( desc.mIdentifier ).mIdentifier != kInvalidElementIdentifier )
		{
			RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Duplicate element desc identifier %s", GetElementString( desc.mIdentifier ).c_str() );
			return false;
		}

		if( innate.empty() || innate.size() > sizeof( InnateIdentifier ) )
		{
			RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line %i, bad innate", math::integer_cast<int>( line ) );
			return false;
		}
		desc.mInnate = MakeInnateIdentifier( innate );

		( rftl::stringstream() << baseLevel ) >> desc.mBaseLevel;
		if( desc.mBaseLevel == 0 )
		{
			RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line %i, bad base level", math::integer_cast<int>( line ) );
			return false;
		}

		( rftl::stringstream() << minLevel ) >> desc.mMinLevel;
		if( desc.mMinLevel == 0 )
		{
			RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line %i, bad min level", math::integer_cast<int>( line ) );
			return false;
		}

		( rftl::stringstream() << maxLevel ) >> desc.mMaxLevel;
		if( desc.mMaxLevel == 0 )
		{
			RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line %i, bad max level", math::integer_cast<int>( line ) );
			return false;
		}

		mElementDescs.emplace_back( rftl::move( desc ) );

		descTable.pop_front();
		line++;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
