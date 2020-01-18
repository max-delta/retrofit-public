#pragma once
#include "project.h"

#include "GameSprite/CharacterCompositor.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/immutable_string.h"
#include "rftl/bitset"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

class GAMESPRITE_API CharacterCreator
{
	RF_NO_COPY( CharacterCreator );

	//
	// Types and constants
public:
	static constexpr size_t kMaxTags = 16;
	using Tags = rftl::static_vector<rftl::immutable_string, kMaxTags>;
	using PieceId = rftl::string;
	using TagBits = rftl::bitset<kMaxTags>;

private:
	using PiecesById = rftl::unordered_map<PieceId, TagBits>;
	using CollectionsByType = rftl::unordered_map<CharacterPieceType, PiecesById>;


	//
	// Public methods
public:
	CharacterCreator( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu );

	bool LoadPieceTables( file::VFSPath const& pieceTablesDir );
	bool LoadCompositionTable( file::VFSPath const& compositionTablePath );

	TagBits GetTagsAsFlags( Tags const& tags ) const;
	PieceId IterateNextValidPiece( PieceId const& iterator, CharacterPieceType type, TagBits flags ) const;

	CompositeCharacter CreateCompositeCharacter( CompositeCharacterParams const& params );


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

///////////////////////////////////////////////////////////////////////////////
}}
