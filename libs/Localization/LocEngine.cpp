#include "stdafx.h"
#include "LocEngine.h"

#include "Localization/LocQuery.h"
#include "Localization/LocResult.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "core_unicode/StringConvert.h"

#include "rftl/sstream"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

bool LocEngine::InitializeFromKeymapFile( file::VFS const& vfs, file::VFSPath const& path, TextDirection textDirection )
{
	mKeymap = {};
	mTextDirection = TextDirection::kInvalid;

	file::FileHandlePtr const keymapHandle = vfs.GetFileForRead( path );
	if( keymapHandle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to get keymap file for read" );
		return false;
	}

	file::FileBuffer const keymapBuffer{ *keymapHandle.Get(), false };
	if( keymapBuffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to get data from keymap file buffer" );
		return false;
	}

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( keymapBuffer.GetChars() );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to read keymap file as csv" );
		return false;
	}

	Keymap newKeymap = {};

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

		rftl::string const& fromVal = fromField;
		if( fromVal.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'from' column", i_row );
			return false;
		}
		if( fromVal.at( 0 ) != '$' )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'from' column, expected to begin with '$'", i_row );
			return false;
		}

		rftl::u32string toVal = unicode::ConvertToUtf32( toField );
		if( toVal.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'to' column", i_row );
			return false;
		}

		if( newKeymap.count( fromVal ) != 0 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a duplicate 'from' value, already seen previously", i_row );
			return false;
		}

		newKeymap[fromVal] = rftl::move( toVal );
	}

	mKeymap = rftl::move( newKeymap );
	mTextDirection = textDirection;
	return true;
}



void LocEngine::SetKeyDebug( bool value )
{
	mKeyDebug = value;
}



TextDirection LocEngine::GetTextDirection() const
{
	return mTextDirection;
}



LocResult LocEngine::Query( LocQuery const& query ) const
{
	if( mKeyDebug )
	{
		// Return key as result
		rftl::string const id = query.mKey.GetAsString();
		rftl::u32string codePoints( id.begin(), id.end() );
		return LocResult( rftl::move( codePoints ) );
	}

	// Lookup
	Keymap::const_iterator const iter = mKeymap.find( query.mKey.GetAsString() );
	if( iter != mKeymap.end() )
	{
		return LocResult( iter->second );
	}
	else
	{
		RFLOG_NOTIFY( nullptr, RFCAT_LOCALIZATION, "Failed to map the key '%s'", query.mKey.GetAsString().c_str() );

		// Return key as result
		rftl::string const id = query.mKey.GetAsString();
		rftl::u32string codePoints( id.begin(), id.end() );
		return LocResult( rftl::move( codePoints ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
