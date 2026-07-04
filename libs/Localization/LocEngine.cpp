#include "stdafx.h"
#include "LocEngine.h"

#include "Localization/LocQuery.h"
#include "Localization/LocResult.h"

#include "PlatformFilesystem/VFS.h"

#include "Serialization/CsvReader.h"

#include "core_unicode/StringConvert.h"
#include "core_unicode/BufferConvert.h"
#include "core_vfs/FileBuffer.h"

#include "rftl/sstream"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr size_t kMaxReplaceDepth = 10;

struct StringSplit
{
	rftl::u32string_view mPreceding;
	rftl::u32string_view mToken;
	rftl::u32string_view mFollowing;
};



static StringSplit FindFirstReplacementToken( rftl::string_view key, rftl::u32string_view str )
{
	RF_ASSERT( str.empty() == false );

	static constexpr rftl::u32string_view kOpen = kReplacementTokenOpen;
	static constexpr rftl::u32string_view kClose = kReplacementTokenClose;

	// Start?
	size_t const startPos = str.find( kOpen );
	if( startPos == rftl::u32string_view::npos )
	{
		return {};
	}
	rftl::u32string_view const preceding = str.substr( 0, startPos );
	rftl::u32string_view const startWindow = str.substr( startPos );

	// End?
	size_t const closingPos = startWindow.find( kClose );
	if( closingPos == rftl::u32string_view::npos )
	{
		RFLOG_NOTIFY( key, RFCAT_LOCALIZATION,
			U"Found an unfinished replacement token in '{}'",
			logging::CppUnicodeFormatWorkaround( str ) );
		return {};
	}
	size_t const lastPos = closingPos + kClose.size();
	rftl::u32string_view const token = startWindow.substr( 0, lastPos );
	rftl::u32string_view const following = startWindow.substr( lastPos );

	// Sanity check
	{
		static constexpr auto beginPtr = []( rftl::u32string_view view ) -> char32_t const*
		{
			return view.data();
		};
		static constexpr auto endPtr = []( rftl::u32string_view view ) -> char32_t const*
		{
			return view.data() + view.size();
		};
		RF_ASSERT( beginPtr( str ) == beginPtr( preceding ) );
		RF_ASSERT( endPtr( preceding ) == beginPtr( token ) );
		RF_ASSERT( endPtr( token ) == beginPtr( following ) );
		RF_ASSERT( endPtr( following ) == endPtr( str ) );
	}

	// Remove the outer syntax, but leave the key prefix
	rftl::u32string_view strippedToken = token;
	static_assert( kOpen.back() == kKeyPrefix );
	strippedToken.remove_prefix( kOpen.size() - 1 );
	strippedToken.remove_suffix( kClose.size() );

	// Return
	return StringSplit{
		.mPreceding = preceding,
		.mToken = strippedToken,
		.mFollowing = following };
}

}
///////////////////////////////////////////////////////////////////////////////

bool LocEngine::InitializeFromKeymapDirectory( file::VFS const& vfs, file::VFSPath const& path, TextDirection textDirection )
{
	mKeymap = {};
	mTextDirection = TextDirection::kInvalid;

	// Enumerate keymaps
	RFLOG_INFO( path, RFCAT_LOCALIZATION, "Initializing LocEngine from keymap directory" );
	rftl::vector<file::VFSPath> keymapFiles;
	vfs.EnumerateDirectoryRecursive(
		path,
		file::VFSMount::Permissions::ReadOnly,
		keymapFiles );

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

	// Expand keys, performing replacement
	{
		size_t const numReplacements = ExpandAndReplaceNestedKeys( newKeymap, details::kMaxReplaceDepth, true );
		RFLOG_INFO( path, RFCAT_LOCALIZATION, "Expanded {} nested keys", numReplacements );
	}

	// Store
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
	if( query.mKey.mID.empty() == false )
	{
		RF_ASSERT( query.mDynamicTarget.empty() );
		return ProcessDirectKeyQuery( query );
	}

	if( query.mDynamicTarget.empty() == false )
	{
		RF_ASSERT( query.mKey.mID.empty() );
		return ProcessDymamicTargetQuery( query );
	}

	RFLOGF_ERROR( nullptr, RFCAT_LOCALIZATION, "Failed to determine what a localization query intended" );
	return LocResult( U"INVALID_QUERY" );
}

///////////////////////////////////////////////////////////////////////////////

LocEngine::Keymap LocEngine::LoadKeymapFromFile( file::VFS const& vfs, file::VFSPath const& path )
{
	RFLOG_DEBUG( path, RFCAT_LOCALIZATION, "Loading keymap from file" );

	char separator = serialization::CsvReader::kDefaultSeparator;
	rftl::string extension = path.GetTrailingExtensions();
	if( extension == ".keymap.psv" )
	{
		// PSV (pipe-separated values)
		// NOTE: This is supported because a lot of text, like dialogues, will
		//  want to use commas
		separator = '|';
	}
	else if( extension == ".keymap.csv" )
	{
		// CSV (comma-separated values)
		// NOTE: This is supported because it's an easy format to open in any
		//  one of a variety of fancy editors, but has the limitation that
		//  commas can't be used in any of the entries
	}
	else
	{
		RFLOG_WARNING( path, RFCAT_LOCALIZATION, "Keymap file has unexpected extension, will assume csv" );
	}

	file::SeekHandlePtr const keymapHandle = vfs.GetFileForRead( path );
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

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( keymapBuffer.GetChars(), separator );
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
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has {} columns, expected 2", i_row, numCols );
			return {};
		}

		rftl::string const& fromField = row.at( 0 );
		if( fromField.empty() )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has an empty 'from' column", i_row );
			return {};
		}

		rftl::string const& toField = row.at( 1 );
		if( toField.empty() )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has an empty 'to' column", i_row );
			return {};
		}

		rftl::string const& fromVal = fromField;
		if( fromVal.empty() )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has a malformed 'from' column", i_row );
			return {};
		}
		if( fromVal.at( 0 ) != kKeyPrefix )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has a malformed 'from' column, expected to begin with '{}'", i_row, kKeyPrefix );
			return {};
		}

		// Interpret the field as UTF-8
		rftl::u8string_view toFieldAsUtf8( reinterpret_cast<char8_t const*>( toField.data() ), toField.size() );
		rftl::u32string toVal = unicode::ConvertToUtf32( toFieldAsUtf8 );
		if( toVal.empty() )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has a malformed 'to' column", i_row );
			return {};
		}

		if( newKeymap.count( fromVal ) != 0 )
		{
			RFLOGF_NOTIFY( path, RFCAT_LOCALIZATION, "Line {} has a duplicate 'from' value, already seen previously", i_row );
			return {};
		}

		newKeymap[fromVal] = rftl::move( toVal );
	}

	return newKeymap;
}



size_t LocEngine::ExpandAndReplaceNestedKeys( Keymap& keymap, size_t maxDepth, bool errorOnMaxDepth )
{
	// Will operate off of an increasingly narrowing subset of entries
	using IterList = rftl::deque<Keymap::iterator>;
	IterList entriesToProcess;

	// First, scan for entries that are relevant, before doing the actual
	//  processing, just to simplify the logic
	for( Keymap::iterator iter = keymap.begin(); iter != keymap.end(); iter++ )
	{
		rftl::u32string_view const token =
			details::FindFirstReplacementToken( iter->first, iter->second ).mToken;
		if( token.empty() )
		{
			continue;
		}

		// NOTE: Non-optimal to have wasted time scanning the string without
		//  processing it yet, but it keeps the logic easier to understand
		entriesToProcess.emplace_back( iter );
	}
	RFLOG_DEBUG( nullptr, RFCAT_LOCALIZATION,
		"Found {} entries that contained atleast one replacement token",
		entriesToProcess.size() );

	// Track number of replacements
	size_t numReplacements = 0;

	// Process in a series of passes, so that we can bail if the space is
	//  becoming too complex and looks like it might be infinitely recursive
	size_t numPasses = 0;
	while( entriesToProcess.empty() == false )
	{
		// Check if we should bail
		if( numPasses >= maxDepth )
		{
			if( errorOnMaxDepth )
			{
				RFLOG_NOTIFY( nullptr, RFCAT_LOCALIZATION,
					"Found {} keys that exceeded the max nesting depth for"
					" replacement tokens, first erroneous key '{}'",
					entriesToProcess.size(),
					( *entriesToProcess.begin() )->first );
			}
			break;
		}

		// For each entry to process...
		size_t indexToProcess = 0;
		while( indexToProcess < entriesToProcess.size() )
		{
			Keymap::iterator& entryToProcess = entriesToProcess.at( indexToProcess );
			rftl::string const& key = entryToProcess->first;
			rftl::u32string& targetString = entryToProcess->second;

			// Attempt a single replacement
			// NOTE: This replacement may just be replacing the token with yet
			//  ANOTHER replacement token, and we are electing not to recurse
			//  here under the hope that a breadth-first approach may reduce
			//  the total number replacements performed, and reduce the total
			//  work, since this approach can collapse a long chain of leaf
			//  nodes that may be used heavily across the whole dataset
			rftl::u32string replaced = ReplaceOneToken( keymap, key, targetString );

			if( replaced.empty() )
			{
				// If replacement failed, assume there's no further
				//  replacements to perform on the entry, and remove it
				// NOTE: Removal via swap-and-pop
				rftl::swap( entryToProcess, entriesToProcess.back() );
				entriesToProcess.pop_back();
				continue;
			}
			else
			{
				// If replacement succeeded, keep it around for another pass in
				//  case there are a chain of further replacements to be made
				//  on it, or more tokens that we didn't handle at this time
				targetString = rftl::move( replaced );
				numReplacements++;
				indexToProcess++;
				continue;
			}
		}
	}

	return numReplacements;
}



size_t LocEngine::ExpandNestedKeys( Keymap const& keymap, rftl::u32string& str, size_t maxDepth, bool errorOnMaxDepth )
{
	// There is no key associated with this operation
	static constexpr rftl::string_view kDeferred = "DEFERRED_EXPANSION";

	// Will modify in-place
	rftl::u32string& targetString = str;

	// Track number of replacements
	size_t numReplacements = 0;

	// Process in a series of passes, so that we can bail if the space is
	//  becoming too complex and looks like it might be infinitely recursive
	size_t numPasses = 0;
	while( true )
	{
		// Check if we should bail
		if( numPasses >= maxDepth )
		{
			if( errorOnMaxDepth )
			{
				RFLOG_NOTIFY( nullptr, RFCAT_LOCALIZATION,
					"Asked to expand a string that exceeded the max nesting"
					" depth for replacement tokens, stopped partway at '{}'",
					logging::CppUnicodeFormatWorkaround( targetString ) );
			}
			break;
		}

		// Attempt a single replacement
		rftl::u32string replaced = ReplaceOneToken( keymap, kDeferred, targetString );

		if( replaced.empty() )
		{
			// If replacement failed, assume there's no further
			//  replacements to perform on the entry, and bail
			break;
		}
		else
		{
			// If replacement succeeded, perform another pass in case there
			//  are a chain of further replacements to be made on it, or
			//  more tokens that we didn't handle at this time
			targetString = rftl::move( replaced );
			numReplacements++;
			continue;
		}
	}

	return numReplacements;
}



rftl::u32string LocEngine::ReplaceOneToken( Keymap const& keyMap, rftl::string_view key, rftl::u32string str )
{
	RF_ASSERT( keyMap.empty() == false );
	RF_ASSERT( str.empty() == false );
	RF_ASSERT( str.empty() == false );

	// Try to find a token to replace
	details::StringSplit const stringSplit = details::FindFirstReplacementToken( key, str );
	if( stringSplit.mToken.empty() )
	{
		return {};
	}

	// Convert token to an ASCII key
	rftl::u32string_view const& u32Token = stringSplit.mToken;
	rftl::array<char, 256> keyBuffer;
	rftl::string_view const bufferView( keyBuffer.begin(), keyBuffer.end() );
	size_t const charsDecoded = unicode::ConvertToASCII( keyBuffer, u32Token );
	if( charsDecoded > keyBuffer.size() )
	{
		RFLOG_NOTIFY( key, RFCAT_LOCALIZATION,
			U"Found a replacement token that is too large to parse (exceeds {} chars): '{}'",
			keyBuffer.size(),
			logging::CppUnicodeFormatWorkaround( u32Token ) );
		return {};
	}
	rftl::string_view const lookupKey = bufferView.substr( 0, charsDecoded );

	// Lookup replacement
	Keymap::const_iterator const lookupIter = keyMap.find( RFTL_STR_V_HASH( lookupKey ) );
	if( lookupIter == keyMap.end() )
	{
		RFLOG_NOTIFY( key, RFCAT_LOCALIZATION,
			U"Could not find a matching key for the replacement token: '{}'",
			lookupKey );
		return {};
	}
	rftl::u32string_view const replacement = lookupIter->second;

	// Form and return the newly-spliced string
	rftl::u32string retVal = {};
	retVal.reserve( stringSplit.mPreceding.size() + replacement.size() + stringSplit.mFollowing.size() );
	retVal.append( stringSplit.mPreceding );
	retVal.append( replacement );
	retVal.append( stringSplit.mFollowing );
	return retVal;
}



LocResult LocEngine::ProcessDirectKeyQuery( LocQuery const& query ) const
{
	if( mKeyDebug )
	{
		// Return key as result
		rftl::string const id = query.mKey.GetAsDiagnosticString();
		rftl::u32string codePoints( id.begin(), id.end() );
		return LocResult( rftl::move( codePoints ) );
	}

	// Lookup
	Keymap::const_iterator const iter = mKeymap.find( RFTL_STR_V_HASH( query.mKey.mID ) );
	if( iter != mKeymap.end() )
	{
		return LocResult( iter->second );
	}
	else
	{
		RFLOGF_NOTIFY( nullptr, RFCAT_LOCALIZATION,
			"Failed to map the key '{}'",
			query.mKey.GetAsDiagnosticString() );

		// Return key as result
		rftl::string_view const& id = query.mKey.mID;
		rftl::u32string codePoints( id.begin(), id.end() );
		return LocResult( rftl::move( codePoints ) );
	}
}



LocResult LocEngine::ProcessDymamicTargetQuery( LocQuery const& query ) const
{
	rftl::u32string target( query.mDynamicTarget );
	RF_ASSERT( target.empty() == false );

	if( mKeyDebug )
	{
		// Return target as result
		return LocResult( rftl::move( target ) );
	}

	// Expand
	size_t const numReplacements = ExpandNestedKeys( mKeymap, target, details::kMaxReplaceDepth, true );
	RFLOG_TRACE( nullptr, RFCAT_LOCALIZATION, "Expanded {} nested keys in dynamic target", numReplacements );
	if( target.empty() )
	{
		RFLOGF_NOTIFY( nullptr, RFCAT_LOCALIZATION,
			"Failed to exapand the target '{}'",
			logging::CppUnicodeFormatWorkaround( query.mDynamicTarget ) );
		return LocResult( U"INVALID_TARGET" );
	}

	// Success!
	return LocResult( rftl::move( target ) );
}

///////////////////////////////////////////////////////////////////////////////
}
