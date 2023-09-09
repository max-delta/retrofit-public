#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/CharacterCreator.h"

#include "PlatformFilesystem/VFS.h"


namespace RF::scratch {
///////////////////////////////////////////////////////////////////////////////
namespace details {

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;

	sprite::CharacterCreator creator( app::gVfs, app::gGraphics );
	bool const loadPieceSuccess = creator.LoadPieceTables(
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "pieces" ) );
	RF_ASSERT( loadPieceSuccess );
	bool const loadCompositionSuccess = creator.LoadCompositionTable(
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "composite.csv" ) );
	RF_ASSERT( loadCompositionSuccess );

#if 0
	sprite::CharacterCreator::TagBits const characterTagBits = creator.GetTagsAsFlags( { "female", "portal" } );

	// Choose first valid
	sprite::CharacterCreator::PieceId const base = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Base, characterTagBits );
	sprite::CharacterCreator::PieceId const top = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Top, characterTagBits );
	sprite::CharacterCreator::PieceId const bottom = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Bottom, characterTagBits );
	sprite::CharacterCreator::PieceId const hair = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Hair, characterTagBits );
	sprite::CharacterCreator::PieceId const species = creator.IterateNextValidPiece( {}, sprite::CharacterPieceType::Species, characterTagBits );
#else
	sprite::CharacterCreator::PieceId const base = "awokenc_f";
	sprite::CharacterCreator::PieceId const top = "shirt_long_f";
	sprite::CharacterCreator::PieceId const bottom = "skirt_long_f";
	sprite::CharacterCreator::PieceId const hair = "short1_f";
	sprite::CharacterCreator::PieceId const species = "awokenc_f";
#endif
	RF_ASSERT( base.empty() == false );
	RF_ASSERT( top.empty() == false );
	RF_ASSERT( bottom.empty() == false );
	RF_ASSERT( hair.empty() == false );
	RF_ASSERT( species.empty() == false );

	file::VFSPath const charPieces = file::VFS::kRoot.GetChild( "assets", "textures", "char" );

	char const* const id = "ID_TEST";
	file::VFSPath const outDir = file::VFS::kRoot.GetChild( "scratch", "composite_char", id );

	sprite::CompositeCharacterParams params = {};
	params.mMode = "36";
	params.mCompositeWidth = 36;
	params.mCompositeHeight = 40;
	params.mBaseId = base;
	params.mTopId = top;
	params.mBottomId = bottom;
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
}
