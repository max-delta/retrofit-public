#include "stdafx.h"
#include "LocEngine.h"

#include "Localization/LocQuery.h"
#include "Localization/LocResult.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "core_unicode/StringConvert.h"

#include "rftl/sstream"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

bool LocEngine::InitializeFromKeymapDirectory( file::VFS const& vfs, file::VFSPath const& path, TextDirection textDirection )
{
	mKeymap = {};
	mTextDirection = TextDirection::kInvalid;

	// Enumerate keymaps
	rftl::vector<file::VFSPath> keymapFiles;
	{
		rftl::vector<file::VFSPath> unexpectedDirectories;
		vfs.EnumerateDirectory(
			path,
			file::VFSMount::Permissions::ReadOnly,
			keymapFiles,
			unexpectedDirectories );
		for( file::VFSPath const& unexpectedDirectory : unexpectedDirectories )
		{
			RFLOG_WARNING( unexpectedDirectory, RFCAT_LOCALIZATION, "Unexpected directory found alongside keymap files" );
		}
	}

	if( keymapFiles.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to find any keymap files" );
		return false;
	}

	// Load all keymaps
	Keymap newKeymap = {};
	for( file::VFSPath const& keymapFile : keymapFiles )
	{
		// Load
		Keymap const loadedKeymap = LoadKeymapFromFile( vfs, keymapFile );
		if( loadedKeymap.empty() )
		{
			RFLOG_NOTIFY( keymapFile, RFCAT_LOCALIZATION, "Couldn't load keymap file" );
			return false;
		}

		// Merge
		newKeymap.reserve( newKeymap.size() + loadedKeymap.size() );
		newKeymap.insert( loadedKeymap.begin(), loadedKeymap.end() );
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

LocEngine::Keymap LocEngine::LoadKeymapFromFile( file::VFS const& vfs, file::VFSPath const& path )
{
	file::FileHandlePtr const keymapHandle = vfs.GetFileForRead( path );
	if( keymapHandle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to get keymap file for read" );
		return {};
	}

	file::FileBuffer const keymapBuffer{ *keymapHandle.Get(), false };
	if( keymapBuffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to get data from keymap file buffer" );
		return {};
	}

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( keymapBuffer.GetChars() );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Failed to read keymap file as csv" );
		return {};
	}

	Keymap newKeymap = {};

	size_t const numRows = csv.size();
	newKeymap.reserve( numRows );
	for( size_t i_row = 0; i_row < numRows; i_row++ )
	{
		rftl::deque<rftl::string> const& row = csv.at( i_row );
		size_t const numCols = row.size();
		if( numCols != 2 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has %llu columns, expected 2", i_row, numCols );
			return {};
		}

		rftl::string const& fromField = row.at( 0 );
		if( fromField.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has an empty 'from' column", i_row );
			return {};
		}

		rftl::string const& toField = row.at( 1 );
		if( toField.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has an empty 'to' column", i_row );
			return {};
		}

		rftl::string const& fromVal = fromField;
		if( fromVal.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'from' column", i_row );
			return {};
		}
		if( fromVal.at( 0 ) != '$' )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'from' column, expected to begin with '$'", i_row );
			return {};
		}

		rftl::u32string toVal = unicode::ConvertToUtf32( toField );
		if( toVal.empty() )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a malformed 'to' column", i_row );
			return {};
		}

		if( newKeymap.count( fromVal ) != 0 )
		{
			RFLOG_NOTIFY( path, RFCAT_LOCALIZATION, "Line %llu has a duplicate 'from' value, already seen previously", i_row );
			return {};
		}

		newKeymap[fromVal] = rftl::move( toVal );
	}

	return newKeymap;
}

///////////////////////////////////////////////////////////////////////////////
}
