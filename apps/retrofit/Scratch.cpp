#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/CharacterCreator.h"

#include "PlatformFilesystem/VFS.h"


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////
namespace details {

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;

	sprite::CharacterCreator creator( app::gVfs, app::gGraphics );
	bool const loadPieceSuccess = creator.LoadPieceTables(
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "pieces.csv" ),
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "pieces" ) );
	RF_ASSERT( loadPieceSuccess );
	bool const loadCompositionSuccess = creator.LoadCompositionTable(
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "composite.csv" ) );
	RF_ASSERT( loadCompositionSuccess );

	sprite::CharacterCreator::TagBits const characterTagBits = creator.GetTagsAsFlags( { "female", "portal" } );

	// Choose first valid
	sprite::CharacterCreator::PieceId const base = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Base, characterTagBits );
	sprite::CharacterCreator::PieceId const clothing = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Clothing, characterTagBits );
	sprite::CharacterCreator::PieceId const hair = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Hair, characterTagBits );
	sprite::CharacterCreator::PieceId const species = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Species, characterTagBits );
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
