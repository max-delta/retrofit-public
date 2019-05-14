#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/CharacterCompositor.h"

#include "PlatformFilesystem/VFS.h"


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////
namespace details {

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;

	file::VFSPath const charPieces = file::VFS::kRoot.GetChild( "assets", "textures", "char" );

	char const* const id = "ID_TODO";
	file::VFSPath const outDir = file::VFS::kRoot.GetChild( "scratch", "char", id );

	sprite::CharacterCompositor compositor( app::gVfs, app::gGraphics );
	bool const loadSuccess = compositor.LoadPieceTables(
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "pieces.csv" ),
		file::VFS::kRoot.GetChild( "assets", "tables", "char", "pieces" ) );
	RF_ASSERT( loadSuccess );

	sprite::CompositeCharacterParams params = {};
	params.mBaseId = "awokenc_f";
	params.mBlothingId = "healer_f";
	params.mHairId = "clothing1_f";
	params.mSpeciesId = "awokenc_f";
	params.mCharPiecesDir = charPieces;
	params.mOutputDir = outDir;
	compositor.CreateCompositeCharacter( params );
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
