#include "stdafx.h"
#include "PageMapper.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "rftl/sstream"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

bool PageMapper::InitializeFromCharmapFile( file::VFS const& vfs, file::VFSPath const& path )
{
	mCharmap = {};

	file::FileHandlePtr const charmapHandle = vfs.GetFileForRead( path );
	if( charmapHandle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to get charmap file for read" );
		return false;
	}

	file::FileBuffer const charmapBuffer{ *charmapHandle.Get(), false };
	if( charmapBuffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to get data from charmap file buffer" );
		return false;
	}

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( charmapBuffer.GetChars() );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to read charmap file as csv" );
		return false;
	}

	Charmap newCharmap = {};

	size_t const numRows = csv.size();
	for( size_t i_row = 0; i_row < numRows; i_row++ )
	{
		rftl::deque<rftl::string> const& row = csv.at( i_row );
		size_t const numCols = row.size();
		if( numCols != 2 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has %llu columns, expected 2", i_row, numCols );
			return false;
		}

		rftl::string const& fromField = row.at( 0 );
		if( fromField.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has an empty 'from' column", i_row );
			return false;
		}

		rftl::string const& toField = row.at( 1 );
		if( toField.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has an empty 'to' column", i_row );
			return false;
		}

		uint32_t fromVal = 0;
		( rftl::stringstream() << fromField ) >> fromVal;
		if( fromVal == 0 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'from' column", i_row );
			return false;
		}

		uint32_t toVal = 0;
		( rftl::stringstream() << toField ) >> toVal;
		if( toVal == 0 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'to' column", i_row );
			return false;
		}
		else if( toVal > 255 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has invalid 'to' column, expected a number between 1 and 255", i_row );
			return false;
		}

		if( newCharmap.count( fromVal ) != 0 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a duplicate 'from' value, already seen previously", i_row );
			return false;
		}

		newCharmap[static_cast<char32_t>( fromVal )] = static_cast<char>( toVal );
	}

	mCharmap = rftl::move( newCharmap );
	return true;
}



rftl::string PageMapper::MapTo8Bit( rftl::u32string const& codePoints ) const
{
	rftl::string retVal;
	retVal.reserve( codePoints.size() );
	for( char32_t const& codePoint : codePoints )
	{
		Charmap::const_iterator const iter = mCharmap.find( codePoint );
		if( iter != mCharmap.end() )
		{
			retVal.push_back( iter->second );
		}
		else
		{
			RFLOG_NOTIFY( nullptr, RFCAT_LOCALIZATION, "Failed to map the character %lu to an 8bit character", static_cast<uint32_t>( codePoint ) );
			retVal.push_back( static_cast<char>( static_cast<unsigned char>( 255 ) ) );
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}
