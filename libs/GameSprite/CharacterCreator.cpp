#include "stdafx.h"
#include "CharacterCreator.h"

#include "Serialization/CsvReader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

CharacterCreator::CharacterCreator( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu )
	: mVfs( vfs )
	, mCompositor( vfs, ppu )
{
	//
}



bool CharacterCreator::LoadPieceTables( file::VFSPath const& masterTablePath, file::VFSPath const& pieceTablesDir )
{
	return mCompositor.LoadPieceTables( masterTablePath, pieceTablesDir );
}



bool CharacterCreator::LoadCompositionTable( file::VFSPath const& compositionTablePath )
{
	// TODO: Ensure piece table is loaded
	// TODO: Load table as tag bits, keyed by type and ID
	// TODO: Vet against piece table while loading

	rftl::deque<rftl::deque<rftl::string>> compositionTable = LoadCSV( compositionTablePath );
	if( compositionTable.empty() )
	{
		RFLOG_NOTIFY( compositionTablePath, RFCAT_GAMESPRITE, "Failed to load composition rules table file" );
		return false;
	}

	size_t line = 0;

	static constexpr size_t kIdentifierFields = 2;

	rftl::deque<rftl::string> const header = compositionTable.front();
	compositionTable.pop_front();
	if( header.size() <= kIdentifierFields || header.at( 0 ) != "type" || header.at( 1 ) != "id" )
	{
		RFLOG_NOTIFY( compositionTablePath, RFCAT_GAMESPRITE, "Malformed composition rules header, expected 'type, id, tags...'" );
		return false;
	}
	line++;

	// Figure out tags from header
	RF_ASSERT( mTags.empty() );
	mTags.clear();
	size_t const numTags = header.size() - kIdentifierFields;
	for( size_t i = 0; i < numTags; i++ )
	{
		rftl::string const& tag = header.at( i + kIdentifierFields );
		for( rftl::immutable_string const& existingTag : mTags )
		{
			if( tag == existingTag )
			{
				RFLOG_NOTIFY( compositionTablePath, RFCAT_GAMESPRITE, "Malformed composition rules header, duplicate tags found" );
				return false;
			}
		}
		mTags.emplace_back( tag );
	}
	RF_ASSERT( mTags.size() == numTags );

	while( compositionTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = compositionTable.front();
		if( entry.size() != kIdentifierFields + numTags )
		{
			RFLOG_NOTIFY( compositionTablePath, RFCAT_GAMESPRITE, "Malformed composition rule entry at line %i, expected number of columns to match header", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& typeString = entry.at( 0 );
		rftl::string const& id = entry.at( 1 );

		CharacterPieceType type = CharacterPieceType::Invalid;
		if( typeString == "base" )
		{
			type = CharacterPieceType::Base;
		}
		else if( typeString == "clothing" )
		{
			type = CharacterPieceType::Clothing;
		}
		else if( typeString == "hair" )
		{
			type = CharacterPieceType::Hair;
		}
		else if( typeString == "species" )
		{
			type = CharacterPieceType::Species;
		}
		else
		{
			RFLOG_NOTIFY( compositionTablePath, RFCAT_GAMESPRITE, "Malformed composition rule entry at line %i, not a valid piece type", math::integer_cast<int>( line ) );
			return false;
		}

		if( id.empty() )
		{
			RFLOG_NOTIFY( compositionTablePath, RFCAT_GAMESPRITE, "Malformed composition rule entry at line %i, bad id", math::integer_cast<int>( line ) );
			return false;
		}

		PiecesById& piecesById = mCollectionsByType[type];
		TagBits& tagBits = piecesById[id];

		tagBits.reset();
		for( size_t i = 0; i < numTags; i++ )
		{
			rftl::string const& tagBit = entry.at( i + kIdentifierFields );
			if( tagBit == "0" )
			{
				tagBits.reset( i );
			}
			else if( tagBit == "1" )
			{
				tagBits.set( i );
			}
			else
			{
				RFLOG_NOTIFY( compositionTablePath, RFCAT_GAMESPRITE, "Malformed composition rule entry at line %i, expected a '0' or a '1'", math::integer_cast<int>( line ) );
				return false;
			}
		}

		compositionTable.pop_front();
		line++;
	}

	return true;
}



CharacterCreator::TagBits CharacterCreator::GetTagsAsFlags( Tags const& tags ) const
{
	TagBits retVal;
	retVal.reset();

	for( Tags::value_type const& tag : tags )
	{
		bool foundTag = false;
		size_t const numBits = mTags.size();
		for( size_t i = 0; i < numBits; i++ )
		{
			Tags::value_type const& bit = mTags.at( i );
			if( bit == tag )
			{
				retVal.set( i );
				foundTag = true;
				break;
			}
		}
		RF_ASSERT( foundTag );
	}
	return retVal;
}



CharacterCreator::PieceId CharacterCreator::IterateNextValidPiece( PieceId const& iterator, CharacterPieceType type, TagBits flags ) const
{
	PiecesById const& piecesByID = mCollectionsByType.at( type );

	PiecesById::const_iterator trueIter;
	if( iterator.empty() )
	{
		// Start at beginning
		trueIter = piecesByID.begin();
	}
	else
	{
		// Start at last iterator
		trueIter = piecesByID.find( iterator );

		// Increment
		if( trueIter != piecesByID.end() )
		{
			trueIter++;
		}
	}

	// Fall forward if missing flags
	while( trueIter != piecesByID.end() && ( trueIter->second & flags ) != flags )
	{
		trueIter++;
	}

	// Return
	if( trueIter != piecesByID.end() )
	{
		return trueIter->first;
	}
	else
	{
		return PieceId();
	}
}



CompositeCharacter CharacterCreator::CreateCompositeCharacter( CompositeCharacterParams const& params )
{
	return mCompositor.CreateCompositeCharacter( params );
}



rftl::deque<rftl::deque<rftl::string>> CharacterCreator::LoadCSV( file::VFSPath const& path )
{
	file::FileHandlePtr const handle = mVfs->GetFileForRead( path );
	if( handle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_GAMESPRITE, "Failed to get file for read" );
		return {};
	}

	file::FileBuffer const buffer{ *handle.Get(), false };
	if( buffer.GetData() == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_GAMESPRITE, "Failed to get data from file buffer" );
		return {};
	}

	char const* const data = reinterpret_cast<char const*>( buffer.GetData() );
	size_t const size = buffer.GetSize();
	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( data, size );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_GAMESPRITE, "Failed to read file as csv" );
		return {};
	}

	return csv;
}

///////////////////////////////////////////////////////////////////////////////
}}
