#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/CharacterCompositor.h"

#include "Serialization/CsvReader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "rftl/extension/static_array.h"
#include "rftl/extension/immutable_string.h"
#include "rftl/bitset"


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////
namespace details {

using namespace sprite;

class CharacterCreator
{
	RF_NO_COPY( CharacterCreator );

	//
	// Types and constants
public:
	static constexpr size_t kMaxTags = 16;
	using Tags = rftl::static_array<rftl::immutable_string, kMaxTags>;
	using PieceId = rftl::immutable_string;
	using TagBits = rftl::bitset<kMaxTags>;
private:
	using PiecesById = rftl::unordered_map<PieceId, TagBits>;
	using CollectionsByType = rftl::unordered_map<CharacterPieceType, PiecesById>;


	//
	// Public methods
public:
	CharacterCreator( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu );

	bool LoadPieceTables( file::VFSPath const& masterTablePath, file::VFSPath const& pieceTablesDir );
	bool LoadCompositionTable( file::VFSPath const& compositionTablePath );

	TagBits GetTagsAsFlags( Tags const& tags ) const;
	PieceId IterateNextValidPiece( PieceId const& iterator, CharacterPieceType type, TagBits flags ) const;

	void CreateCompositeCharacter( CompositeCharacterParams const& params );


	//
	// Private methods
private:
	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	sprite::CharacterCompositor mCompositor;

	Tags mTags;
	CollectionsByType mCollectionsByType;
};



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
	while( trueIter != piecesByID.end() && (trueIter->second & flags) != flags )
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



void CharacterCreator::CreateCompositeCharacter( CompositeCharacterParams const& params )
{
	mCompositor.CreateCompositeCharacter( params );
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

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;

	details::CharacterCreator creator( app::gVfs, app::gGraphics );
	bool const loadPieceSuccess = creator.LoadPieceTables(
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "pieces.csv" ),
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "pieces" ) );
	RF_ASSERT( loadPieceSuccess );
	bool const loadCompositionSuccess = creator.LoadCompositionTable(
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "composite.csv" ) );
	RF_ASSERT( loadCompositionSuccess );

	details::CharacterCreator::TagBits const characterTagBits = creator.GetTagsAsFlags( { "female", "portal" } );

	// Choose first valid
	details::CharacterCreator::PieceId const base = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Base, characterTagBits );
	details::CharacterCreator::PieceId const clothing = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Clothing, characterTagBits );
	details::CharacterCreator::PieceId const hair = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Hair, characterTagBits );
	details::CharacterCreator::PieceId const species = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Species, characterTagBits );
	RF_ASSERT( base.empty() == false );
	RF_ASSERT( clothing.empty() == false );
	RF_ASSERT( hair.empty() == false );
	RF_ASSERT( species.empty() == false );

	file::VFSPath const charPieces = file::VFS::kRoot.GetChild( "assets", "textures", "char" );

	char const* const id = "ID_TODO";
	file::VFSPath const outDir = file::VFS::kRoot.GetChild( "scratch", "char", id );

	sprite::CompositeCharacterParams params = {};
	params.mBaseId = base;
	params.mClothingId = clothing;
	params.mHairId = hair;
	params.mSpeciesId = species;
	params.mCharPiecesDir = charPieces;
	params.mOutputDir = outDir;
	creator.CreateCompositeCharacter( params );
}



void Run()
{
	using namespace details;
}



void End()
{
	using namespace details;
}

///////////////////////////////////////////////////////////////////////////////
}}
