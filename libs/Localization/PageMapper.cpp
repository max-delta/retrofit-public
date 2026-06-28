#include "stdafx.h"
#include "PageMapper.h"

#include "PlatformFilesystem/VFS.h"

#include "Serialization/CsvReader.h"

#include "core_unicode/CodeAreas.h"
#include "core_vfs/FileBuffer.h"

#include "rftl/sstream"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

bool PageMapper::InitializeFromCharmapFile( file::VFS const& vfs, file::VFSPath const& path )
{
	mCharmap = {};

	file::SeekHandlePtr const charmapHandle = vfs.GetFileForRead( path );
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
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has {} columns, expected 2", i_row, numCols );
			return false;
		}

		rftl::string const& fromField = row.at( 0 );
		if( fromField.empty() )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has an empty 'from' column", i_row );
			return false;
		}

		rftl::string const& toField = row.at( 1 );
		if( toField.empty() )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has an empty 'to' column", i_row );
			return false;
		}

		uint32_t fromVal = 0;
		( rftl::stringstream() << fromField ) >> fromVal;
		if( fromVal == 0 )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has a malformed 'from' column", i_row );
			return false;
		}

		uint32_t toVal = 0;
		( rftl::stringstream() << toField ) >> toVal;
		if( toVal == 0 )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has a malformed 'to' column", i_row );
			return false;
		}
		else if( toVal > 255 )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has invalid 'to' column, expected a number between 1 and 255", i_row );
			return false;
		}

		if( newCharmap.count( fromVal ) != 0 )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has a duplicate 'from' value, already seen previously", i_row );
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
		// Handle special bypass case, intended for use with terminal control
		//  sequences that are non-printing, and need to circumvent the normal
		//  mapping operation so they can make it to their terminal parser
		//  while still intact
		{
			using namespace unicode;
			static_assert(
				DeterminePrivateUseArea( kPageMapperCollapseArea << 8 ) == kPageMapperCollapseArea,
				"Page mapper collapse area is not a private use area" );
			PrivateUseArea::Value const privateUseArea = DeterminePrivateUseArea( codePoint );
			if( privateUseArea == kPageMapperCollapseArea )
			{
				char const collapsed = static_cast<char>( codePoint & 0xFFu );
				retVal.push_back( collapsed );
				continue;
			}
		}

		Charmap::const_iterator const iter = mCharmap.find( codePoint );
		if( iter != mCharmap.end() )
		{
			char const& mapped = iter->second;
			retVal.push_back( mapped );
			continue;
		}
		else
		{
			RFLOGF_NOTIFY( nullptr, RFCAT_LOCALIZATION, "Failed to map the character {} to an 8bit character", static_cast<uint32_t>( codePoint ) );
			static constexpr char kFallback = static_cast<char>( static_cast<unsigned char>( 255 ) );
			retVal.push_back( kFallback );
			continue;
		}
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
