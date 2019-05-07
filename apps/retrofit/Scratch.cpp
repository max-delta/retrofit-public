#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/Bitmap.h"
#include "GameSprite/BitmapReader.h"
#include "GameSprite/BitmapWriter.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////
namespace details {

struct CompositeFrameParams
{
	size_t mTileWidth = 0;
	size_t mTileHeight = 0;

	size_t mColumn = 0;

	sprite::Bitmap const* mBaseTex = nullptr;
	size_t mBaseRow = 0;

	sprite::Bitmap const* mClothingTex = nullptr;
	size_t mClothingRow = 0;

	sprite::Bitmap const* mHairTex = nullptr;
	size_t mHairRow = 0;

	sprite::Bitmap const* mSpeciesTex = nullptr;
	size_t mSpeciesNearRow = 0;
	size_t mSpeciesFarRow = 0;
	size_t mSpeciesTailRow = 0;
};



sprite::Bitmap CreateCompositeFrame( CompositeFrameParams const& params )
{
	sprite::Bitmap baseFrame = params.mBaseTex->ExtractRegion(
		params.mTileWidth * params.mColumn,
		params.mTileHeight * params.mBaseRow,
		params.mTileWidth,
		params.mTileHeight );

	sprite::Bitmap clothingFrame = params.mClothingTex->ExtractRegion(
		params.mTileWidth * params.mColumn,
		params.mTileHeight * params.mClothingRow,
		params.mTileWidth,
		params.mTileHeight );

	sprite::Bitmap hairFrame = params.mHairTex->ExtractRegion(
		params.mTileWidth * params.mColumn,
		params.mTileHeight * params.mHairRow,
		params.mTileWidth,
		params.mTileHeight );

	sprite::Bitmap speciesNearFrame = params.mSpeciesTex->ExtractRegion(
		params.mTileWidth * params.mColumn,
		params.mTileHeight * params.mSpeciesNearRow,
		params.mTileWidth,
		params.mTileHeight );

	sprite::Bitmap speciesFarFrame = params.mSpeciesTex->ExtractRegion(
		params.mTileWidth * params.mColumn,
		params.mTileHeight * params.mSpeciesFarRow,
		params.mTileWidth,
		params.mTileHeight );

	sprite::Bitmap speciesTailFrame = params.mSpeciesTex->ExtractRegion(
		params.mTileWidth * params.mColumn,
		params.mTileHeight * params.mSpeciesTailRow,
		params.mTileWidth,
		params.mTileHeight );

	// Composite
	sprite::Bitmap composite( params.mTileWidth, params.mTileHeight );
	{
		static constexpr sprite::Bitmap::ElementType::ElementType kMinAlpha = 1;
		composite.ApplyStencilOverwrite( speciesFarFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( baseFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( clothingFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( hairFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( speciesTailFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( speciesNearFrame, kMinAlpha );
	}

	// Set transparency color for external editors (for debugging)
	math::Color4u8 transparencyColor = math::Color4u8::kMagenta;
	transparencyColor.a = 0;
	composite.ReplaceEachByAlpha( 0, transparencyColor );

	return composite;
}



void WriteFrameToDisk( sprite::Bitmap const& frame, file::VFSPath const& path )
{
	file::VFS* vfs = app::gVfs;

	// Write
	{
		rftl::vector<uint8_t> const toWrite = sprite::BitmapWriter::WriteRGBABitmap( frame.GetData(), frame.GetWidth(), frame.GetHeight() );
		file::FileHandlePtr fileHandle = vfs->GetFileForWrite( path );
		FILE* const file = fileHandle->GetFile();
		fwrite( toWrite.data(), sizeof( uint8_t ), toWrite.size(), file );
	}

	// HACK: Verify it loads
	// TODO: Remove this
	{
		file::FileBuffer const verifyBuffer( *vfs->GetFileForRead( path ), false );
		sprite::Bitmap const verifyBitmap = sprite::BitmapReader::ReadRGBABitmap( verifyBuffer.GetData(), verifyBuffer.GetSize() );
	}
}

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;

	file::VFS* vfs = app::gVfs;

	file::VFSPath const charPieces = file::VFS::kRoot.GetChild( "assets", "textures", "char" );

	file::VFSPath const basePieces = charPieces.GetChild( "base_16_18.png" );
	file::VFSPath const clothingPieces = charPieces.GetChild( "clothing_16_18.png" );
	file::VFSPath const hairPieces = charPieces.GetChild( "hair_16_18.png" );
	file::VFSPath const speciesPieces = charPieces.GetChild( "species_16_18.png" );

	file::VFSPath const outDir = file::VFS::kRoot.GetChild( "scratch" );

	// Load base texture
	sprite::Bitmap baseTex( ExplicitDefaultConstruct{} );
	{
		file::FileBuffer const buffer( *vfs->GetFileForRead( basePieces ), false );
		baseTex = sprite::BitmapReader::ReadRGBABitmap( buffer.GetData(), buffer.GetSize() );
	}

	// Load clothing texture
	sprite::Bitmap clothingTex( ExplicitDefaultConstruct{} );
	{
		file::FileBuffer const buffer( *vfs->GetFileForRead( clothingPieces ), false );
		clothingTex = sprite::BitmapReader::ReadRGBABitmap( buffer.GetData(), buffer.GetSize() );
	}

	// Load hair texture
	sprite::Bitmap hairTex( ExplicitDefaultConstruct{} );
	{
		file::FileBuffer const buffer( *vfs->GetFileForRead( hairPieces ), false );
		hairTex = sprite::BitmapReader::ReadRGBABitmap( buffer.GetData(), buffer.GetSize() );
	}

	// Load species texture
	sprite::Bitmap speciesTex( ExplicitDefaultConstruct{} );
	{
		file::FileBuffer const buffer( *vfs->GetFileForRead( speciesPieces ), false );
		speciesTex = sprite::BitmapReader::ReadRGBABitmap( buffer.GetData(), buffer.GetSize() );
	}

	details::CompositeFrameParams params = {};
	params.mTileWidth = 16;
	params.mTileHeight = 18;
	params.mBaseTex = &baseTex;
	params.mBaseRow = 1;
	params.mClothingTex = &clothingTex;
	params.mClothingRow = 13;
	params.mHairTex = &hairTex;
	params.mHairRow = 3;
	params.mSpeciesTex = &speciesTex;
	params.mSpeciesNearRow = 3;
	params.mSpeciesFarRow = 4;
	params.mSpeciesTailRow = 5;

	params.mColumn = 12;

	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_north0.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_north1.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_north2.bmp" ) );
	params.mColumn++;

	params.mColumn++;

	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_east0.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_east1.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_east2.bmp" ) );
	params.mColumn++;

	params.mColumn++;

	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_south0.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_south1.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_south2.bmp" ) );
	params.mColumn++;

	params.mColumn++;

	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_west0.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_west1.bmp" ) );
	params.mColumn++;
	WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( "test_west2.bmp" ) );
	params.mColumn++;
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
