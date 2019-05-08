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
	enum class SequenceType : uint8_t
	{
		Invalid = 0,

		// Top-down
		// Cardinal directions [N; E; S; W]
		// Walk cycles only
		// 4 frames, 3 unique [Step 1; Mid-step; Step 2]
		// Empty tile between contiguous sequences
		N3_E3_S3_W3
	};

	SequenceType mSequenceType = SequenceType::Invalid;

	size_t mTileWidth = 0;
	size_t mTileHeight = 0;

	size_t mBaseRow = 0;

	size_t mClothingNearRow = 0;
	size_t mClothingFarRow = 0;

	size_t mHairNearRow = 0;
	size_t mHairFarRow = 0;

	size_t mSpeciesNearRow = 0;
	size_t mSpeciesFarRow = 0;
	size_t mSpeciesTailRow = 0;
};

struct CompositeFrameParams
{
	CompositeSequenceParams mSequence = {};

	sprite::Bitmap const* mBaseTex = nullptr;
	sprite::Bitmap const* mClothingTex = nullptr;
	sprite::Bitmap const* mHairTex = nullptr;
	sprite::Bitmap const* mSpeciesTex = nullptr;

	size_t mColumn = 0;
};

struct CompositeAnimParams
{
	enum class AnimKey : uint8_t
	{
		Invalid = 0,
		NWalk,
		EWalk,
		SWalk,
		WWalk
	};

	CompositeSequenceParams mSequence = {};

	file::VFSPath mBasePieces = {};
	file::VFSPath mClothingPieces = {};
	file::VFSPath mHairPieces = {};
	file::VFSPath mSpeciesPieces = {};

	file::VFSPath mTextureOutputDirectory = {};

	rftl::unordered_map<AnimKey, file::VFSPath> mOutputPaths = {};
};



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



class CharacterCompositor
{
	RF_NO_COPY( CharacterCompositor );

public:
	CharacterCompositor( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu );

	void Wip( file::VFSPath const& charPieces, file::VFSPath const& outDir );


private:
	static sprite::Bitmap CreateCompositeFrame( CompositeFrameParams const& params );
	void WriteFrameToDisk( sprite::Bitmap const& frame, file::VFSPath const& path );
	void CreateCompositeAnims( CompositeAnimParams const& params );

private:
	WeakPtr<file::VFS const> mVfs;
	WeakPtr<gfx::PPUController> mPpu;
	BitmapCache mBitmapCache;
};


CharacterCompositor::CharacterCompositor( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu )
	: mVfs( vfs )
	, mPpu( ppu )
	, mBitmapCache( vfs )
{
	//
}



void CharacterCompositor::Wip( file::VFSPath const& charPieces, file::VFSPath const& outDir )
{
	CompositeAnimParams animParams = {};
	animParams.mSequence.mSequenceType = CompositeSequenceParams::SequenceType::N3_E3_S3_W3;
	animParams.mSequence.mTileWidth = 16;
	animParams.mSequence.mTileHeight = 18;
	animParams.mSequence.mBaseRow = 1;
	animParams.mSequence.mClothingNearRow = 13;
	animParams.mSequence.mClothingFarRow = 0;
	animParams.mSequence.mHairNearRow = 3;
	animParams.mSequence.mHairFarRow = 0;
	animParams.mSequence.mSpeciesNearRow = 3;
	animParams.mSequence.mSpeciesFarRow = 4;
	animParams.mSequence.mSpeciesTailRow = 5;
	animParams.mBasePieces = charPieces.GetChild( "base_16_18.png" );
	animParams.mClothingPieces = charPieces.GetChild( "clothing_16_18.png" );
	animParams.mHairPieces = charPieces.GetChild( "hair_16_18.png" );
	animParams.mSpeciesPieces = charPieces.GetChild( "species_16_18.png" );
	animParams.mTextureOutputDirectory = outDir;
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::NWalk] = outDir.GetChild( "n.fpack" );
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::EWalk] = outDir.GetChild( "e.fpack" );
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::SWalk] = outDir.GetChild( "s.fpack" );
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::WWalk] = outDir.GetChild( "w.fpack" );

	CreateCompositeAnims( animParams );
}



sprite::Bitmap CharacterCompositor::CreateCompositeFrame( CompositeFrameParams const& params )
{
	sprite::Bitmap baseFrame = params.mBaseTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mBaseRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap clothingNearFrame = params.mClothingTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mClothingNearRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap clothingFarFrame = params.mClothingTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mClothingFarRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap hairNearFrame = params.mHairTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mHairNearRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap hairFarFrame = params.mHairTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mHairFarRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesNearFrame = params.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mSpeciesNearRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesFarFrame = params.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mSpeciesFarRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesTailFrame = params.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * params.mColumn,
		params.mSequence.mTileHeight * params.mSequence.mSpeciesTailRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	// Composite
	sprite::Bitmap composite( params.mSequence.mTileWidth, params.mSequence.mTileHeight );
	{
		static constexpr sprite::Bitmap::ElementType::ElementType kMinAlpha = 1;
		composite.ApplyStencilOverwrite( speciesFarFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( hairFarFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( clothingFarFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( baseFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( clothingNearFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( hairNearFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( speciesTailFrame, kMinAlpha );
		composite.ApplyStencilOverwrite( speciesNearFrame, kMinAlpha );
	}

	// Set transparency color for external editors (for debugging)
	math::Color4u8 transparencyColor = math::Color4u8::kMagenta;
	transparencyColor.a = 0;
	composite.ReplaceEachByAlpha( 0, transparencyColor );

	return composite;
}



void CharacterCompositor::WriteFrameToDisk( sprite::Bitmap const& frame, file::VFSPath const& path )
{
	rftl::vector<uint8_t> const toWrite = sprite::BitmapWriter::WriteRGBABitmap( frame.GetData(), frame.GetWidth(), frame.GetHeight() );
	file::FileHandlePtr fileHandle = mVfs->GetFileForWrite( path );
	FILE* const file = fileHandle->GetFile();
	fwrite( toWrite.data(), sizeof( uint8_t ), toWrite.size(), file );
}



void CharacterCompositor::CreateCompositeAnims( CompositeAnimParams const& params )
{
	// HACK: Direct access to texture manager
	// TODO: Re-visit API surface
	gfx::TextureManager& texMan = *mPpu->DebugGetTextureManager();

	// Load textures
	WeakPtr<sprite::Bitmap const> baseTex = mBitmapCache.Fetch( params.mBasePieces );
	WeakPtr<sprite::Bitmap const> clothingTex = mBitmapCache.Fetch( params.mClothingPieces );
	WeakPtr<sprite::Bitmap const> hairTex = mBitmapCache.Fetch( params.mHairPieces );
	WeakPtr<sprite::Bitmap const> speciesTex = mBitmapCache.Fetch( params.mSpeciesPieces );

	CompositeFrameParams frameParams = {};
	frameParams.mSequence = params.mSequence;
	frameParams.mBaseTex = baseTex;
	frameParams.mClothingTex = clothingTex;
	frameParams.mHairTex = hairTex;
	frameParams.mSpeciesTex = speciesTex;

	RF_ASSERT_MSG( params.mSequence.mSequenceType == CompositeSequenceParams::SequenceType::N3_E3_S3_W3, "Only 'N3_E3_S3_W3' is currently supported" );

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
		frameParams.mColumn = startColumn + kColumnOffsets[i];
		WriteFrameToDisk( CreateCompositeFrame( frameParams ), params.mTextureOutputDirectory.GetChild( kFrameNames[i] ) );
	}

	static constexpr size_t kAnimFramesPerDirection = 4;
	static constexpr size_t kTotalFramepacks = 4;
	static constexpr CompositeAnimParams::AnimKey kFramepackKeys[kTotalFramepacks] = {
		CompositeAnimParams::AnimKey::NWalk,
		CompositeAnimParams::AnimKey::EWalk,
		CompositeAnimParams::AnimKey::SWalk,
		CompositeAnimParams::AnimKey::WWalk
	};
	static constexpr size_t kFramepackSourceFrames[kTotalFramepacks][kAnimFramesPerDirection] = {
		{ 0, 1, 2, 1 },
		{ 3, 4, 5, 4 },
		{ 6, 7, 8, 7 },
		{ 9, 10, 11, 10 }
	};

	// Create framepacks
	for( size_t i_framepack = 0; i_framepack < kTotalFramepacks; i_framepack++ )
	{
		size_t const( &sourceFrames )[kAnimFramesPerDirection] = kFramepackSourceFrames[i_framepack];
		CompositeAnimParams::AnimKey const& framepackKey = kFramepackKeys[i_framepack];
		file::VFSPath const& framepackPath = params.mOutputPaths.at( framepackKey );

		// Create framepack
		gfx::FramePack_256 newFPack = {};
		newFPack.mNumTimeSlots = kAnimFramesPerDirection;
		newFPack.mPreferredSlowdownRate = 20;
		for( size_t i = 0; i < kAnimFramesPerDirection; i++ )
		{
			file::VFSPath const frameTexture = params.mTextureOutputDirectory.GetChild( kFrameNames[sourceFrames[i]] );
			newFPack.GetMutableTimeSlots()[i].mTextureReference = texMan.LoadNewResourceGetID( frameTexture );
			newFPack.GetMutableTimeSlotSustains()[i] = 1;
		}

		// Write
		{
			rftl::vector<uint8_t> toWrite;
			bool const writeSuccess = gfx::FramePackSerDes::SerializeToBuffer( texMan, toWrite, newFPack );
			RF_ASSERT( writeSuccess );
			file::FileHandlePtr const fileHandle = mVfs->GetFileForWrite( framepackPath );
			FILE* const file = fileHandle->GetFile();
			fwrite( toWrite.data(), sizeof( uint8_t ), toWrite.size(), file );
		}

		// Release textures
		for( size_t i = 0; i < kAnimFramesPerDirection; i++ )
		{
			file::VFSPath const frameTexture = params.mTextureOutputDirectory.GetChild( kFrameNames[sourceFrames[i]] );
			texMan.DestroyResource( frameTexture.CreateString() );
		}
	}
}

}
///////////////////////////////////////////////////////////////////////////////

void Start()
{
	using namespace details;

	file::VFSPath const charPieces = file::VFS::kRoot.GetChild( "assets", "textures", "char" );

	char const* const id = "ID_TODO";
	file::VFSPath const outDir = file::VFS::kRoot.GetChild( "scratch", "char", id );

	CharacterCompositor compositor( app::gVfs, app::gGraphics );
	compositor.Wip( charPieces, outDir );
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
