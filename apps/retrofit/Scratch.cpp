#include "stdafx.h"
#include "Scratch.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/Bitmap.h"
#include "GameSprite/BitmapReader.h"
#include "GameSprite/BitmapWriter.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "PPU/FramePack.h"
#include "PPU/FramePackSerDes.h"
#include "PPU/PPUController.h"
#include "PPU/TextureManager.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////
namespace details {

struct CompositeSequenceParams
{
	size_t mTileWidth = 0;
	size_t mTileHeight = 0;

	file::VFSPath mBasePieces = {};
	sprite::Bitmap const* mBaseTex = nullptr;
	size_t mBaseRow = 0;

	file::VFSPath mClothingPieces = {};
	sprite::Bitmap const* mClothingTex = nullptr;
	size_t mClothingRow = 0;

	file::VFSPath mHairPieces = {};
	sprite::Bitmap const* mHairTex = nullptr;
	size_t mHairRow = 0;

	file::VFSPath mSpeciesPieces = {};
	sprite::Bitmap const* mSpeciesTex = nullptr;
	size_t mSpeciesNearRow = 0;
	size_t mSpeciesFarRow = 0;
	size_t mSpeciesTailRow = 0;
};

struct CompositeFrameParams
{
	CompositeSequenceParams mSequence = {};

	size_t mColumn = 0;
};



sprite::Bitmap CreateCompositeFrame( CompositeFrameParams const& params )
{
	sprite::Bitmap baseFrame = params.mSequence.mBaseTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mBaseRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap clothingFrame = params.mSequence.mClothingTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mClothingRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap hairFrame = params.mSequence.mHairTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mHairRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesNearFrame = params.mSequence.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mSpeciesNearRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesFarFrame = params.mSequence.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mSpeciesFarRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesTailFrame = params.mSequence.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mSpeciesTailRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	// Composite
	sprite::Bitmap composite( params.mSequence.mTileWidth, params.mSequence.mTileHeight );
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



class BitmapCache
{
	RF_NO_COPY( BitmapCache );

private:
	using CachedBitmaps = rftl::unordered_map<file::VFSPath, UniquePtr<sprite::Bitmap>>;

public:
	explicit BitmapCache( WeakPtr<file::VFS const> vfs );

	WeakPtr<sprite::Bitmap const> Fetch( file::VFSPath const& path );

private:
	WeakPtr<file::VFS const> const mVfs;
	CachedBitmaps mCachedBitmaps;
};



BitmapCache::BitmapCache( WeakPtr<file::VFS const> vfs )
	: mVfs( vfs )
{
	//
}


WeakPtr<sprite::Bitmap const> BitmapCache::Fetch( file::VFSPath const& path )
{
	CachedBitmaps::const_iterator const iter = mCachedBitmaps.find( path );
	if( iter != mCachedBitmaps.end() )
	{
		// Return cached
		RF_ASSERT( iter->second != nullptr );
		return iter->second;
	}

	// Load to cache
	UniquePtr<sprite::Bitmap> bitmap = DefaultCreator<sprite::Bitmap>::Create( ExplicitDefaultConstruct{} );
	WeakPtr<sprite::Bitmap const> retVal = bitmap;
	{
		file::FileHandlePtr const handle = mVfs->GetFileForRead( path );
		RF_ASSERT( handle != nullptr );
		file::FileBuffer const buffer( *handle, false );
		*bitmap = sprite::BitmapReader::ReadRGBABitmap( buffer.GetData(), buffer.GetSize() );
	}
	mCachedBitmaps[path] = rftl::move( bitmap );
	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;

	file::VFSPath const charPieces = file::VFS::kRoot.GetChild( "assets", "textures", "char" );

	file::VFSPath const basePieces = charPieces.GetChild( "base_16_18.png" );
	file::VFSPath const clothingPieces = charPieces.GetChild( "clothing_16_18.png" );
	file::VFSPath const hairPieces = charPieces.GetChild( "hair_16_18.png" );
	file::VFSPath const speciesPieces = charPieces.GetChild( "species_16_18.png" );

	char const* const id = "ID_TODO";

	file::VFSPath const outDir = file::VFS::kRoot.GetChild( "scratch", "char", id );

	details::BitmapCache bitmapCache( app::gVfs );

	// Load textures
	WeakPtr<sprite::Bitmap const> baseTex = bitmapCache.Fetch( basePieces );
	WeakPtr<sprite::Bitmap const> clothingTex = bitmapCache.Fetch( clothingPieces );
	WeakPtr<sprite::Bitmap const> hairTex = bitmapCache.Fetch( hairPieces );
	WeakPtr<sprite::Bitmap const> speciesTex = bitmapCache.Fetch( speciesPieces );

	details::CompositeFrameParams params = {};
	params.mSequence.mTileWidth = 16;
	params.mSequence.mTileHeight = 18;
	params.mSequence.mBaseTex = baseTex;
	params.mSequence.mBaseRow = 1;
	params.mSequence.mClothingTex = clothingTex;
	params.mSequence.mClothingRow = 13;
	params.mSequence.mHairTex = hairTex;
	params.mSequence.mHairRow = 3;
	params.mSequence.mSpeciesTex = speciesTex;
	params.mSequence.mSpeciesNearRow = 3;
	params.mSequence.mSpeciesFarRow = 4;
	params.mSequence.mSpeciesTailRow = 5;
	size_t const startColumn = 12;

	static constexpr size_t kBitmapFramesPerDirection = 3;
	static constexpr size_t kTotalFrames = 4 * kBitmapFramesPerDirection;
	static constexpr char const* kFrameNames[kTotalFrames] = {
		"n0.bmp", "n1.bmp", "n2.bmp",
		"e0.bmp", "e1.bmp", "e2.bmp",
		"s0.bmp", "s1.bmp", "s2.bmp",
		"w0.bmp", "w1.bmp", "w2.bmp"
	};
	static constexpr size_t kColumnOffsets[kTotalFrames] = {
		0, 1, 2,
		4, 5, 6,
		8, 9, 10,
		12, 13, 14
	};

	// Write frames to disk
	for( size_t i = 0; i < kTotalFrames; i++ )
	{
		params.mColumn = startColumn + kColumnOffsets[i];
		WriteFrameToDisk( details::CreateCompositeFrame( params ), outDir.GetChild( kFrameNames[i] ) );
	}

	static constexpr size_t kAnimFramesPerDirection = 4;
	static constexpr size_t kTotalFramepacks = 4;
	static constexpr char const* kFramepackNames[kTotalFramepacks] = {
		"n.fpack", "e.fpack", "s.fpack", "w.fpack"
	};
	static constexpr size_t kFramepackSourceFrames[kTotalFramepacks][kAnimFramesPerDirection] = {
		{ 0, 1, 2, 1 },
		{ 3, 4, 5, 4 },
		{ 6, 7, 8, 7 },
		{ 9, 10, 11, 10 }
	};

	// Create framepacks
	file::VFS* const vfs = app::gVfs;
	gfx::PPUController* const ppu = app::gGraphics;
	gfx::TextureManager& texMan = *ppu->DebugGetTextureManager();
	for( size_t i_framepack = 0; i_framepack < kTotalFramepacks; i_framepack++ )
	{
		size_t const( &sourceFrames )[kAnimFramesPerDirection] = kFramepackSourceFrames[i_framepack];
		char const* const framepackName = kFramepackNames[i_framepack];

		// Create framepack
		UniquePtr<gfx::FramePackBase> newFPack = DefaultCreator<gfx::FramePack_256>::Create();
		newFPack->mNumTimeSlots = kAnimFramesPerDirection;
		newFPack->mPreferredSlowdownRate = 20;
		for( size_t i = 0; i < kAnimFramesPerDirection; i++ )
		{
			file::VFSPath const frameTexture = outDir.GetChild( kFrameNames[sourceFrames[i]] );
			newFPack->GetMutableTimeSlots()[i].mTextureReference = texMan.LoadNewResourceGetID( frameTexture );
			newFPack->GetMutableTimeSlotSustains()[i] = 1;
		}

		// Write
		{
			rftl::vector<uint8_t> toWrite;
			bool const writeSuccess = gfx::FramePackSerDes::SerializeToBuffer( texMan, toWrite, *newFPack );
			RF_ASSERT( writeSuccess );
			file::FileHandlePtr fileHandle = vfs->GetFileForWrite( outDir.GetChild( framepackName ) );
			FILE* const file = fileHandle->GetFile();
			fwrite( toWrite.data(), sizeof( uint8_t ), toWrite.size(), file );
		}
	}
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
