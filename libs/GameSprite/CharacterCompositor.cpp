#include "stdafx.h"
#include "CharacterCompositor.h"

#include "GameSprite/Bitmap.h"
#include "GameSprite/BitmapWriter.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "PPU/PPUController.h"
#include "PPU/FramePack.h"
#include "PPU/FramePackSerDes.h"
#include "PPU/TextureManager.h"

#include "Serialization/CsvReader.h"

#include "rftl/sstream"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

CharacterCompositor::CharacterCompositor( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu )
	: mVfs( vfs )
	, mPpu( ppu )
	, mBitmapCache( vfs )
{
	//
}



bool CharacterCompositor::LoadPieceTables( file::VFSPath const& masterTablePath, file::VFSPath const& pieceTablesDir )
{
	rftl::deque<rftl::deque<rftl::string>> masterTable = LoadCSV( masterTablePath );
	if( masterTable.empty() )
	{
		RFLOG_NOTIFY( masterTablePath, RFCAT_GAMESPRITE, "Failed to load master piece table file" );
		return false;
	}

	size_t line = 0;

	rftl::deque<rftl::string> const header = masterTable.front();
	masterTable.pop_front();
	if( header.size() != 2 || header.at( 0 ) != "type" || header.at( 1 ) != "vfs suffix" )
	{
		RFLOG_NOTIFY( masterTablePath, RFCAT_GAMESPRITE, "Malformed master piece header, expected 'type, vfs suffix'" );
		return false;
	}
	line++;

	while( masterTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = masterTable.front();
		if( entry.size() != 2 )
		{
			RFLOG_NOTIFY( masterTablePath, RFCAT_GAMESPRITE, "Malformed master piece entry at line %i, expected 2 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& typeString = entry.at( 0 );
		rftl::string const& vfsSuffix = entry.at( 1 );

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
			RFLOG_NOTIFY( masterTablePath, RFCAT_GAMESPRITE, "Malformed master piece entry at line %i, not a valid piece type", math::integer_cast<int>( line ) );
			return false;
		}

		file::VFSPath const pieceTablePath = pieceTablesDir.GetChild( vfsSuffix );

		bool loadResult = LoadPieceTable( type, pieceTablePath );
		if( loadResult == false )
		{
			RFLOG_NOTIFY( masterTablePath, RFCAT_GAMESPRITE, "Couldn't load piece table at line %i of master file", math::integer_cast<int>( line ) );
			return false;
		}

		masterTable.pop_front();
		line++;
	}

	return true;
}



void CharacterCompositor::CreateCompositeCharacter( CompositeCharacterParams const& params )
{
	// TODO: Better error-reporting

	CharacterPiece const basePiece = mCharacterPieceCategories.mCollectionsByType.at( CharacterPieceType::Base ).mPiecesById.at( params.mBaseId );
	CharacterPiece const clothingPiece = mCharacterPieceCategories.mCollectionsByType.at( CharacterPieceType::Clothing ).mPiecesById.at( params.mClothingId );
	CharacterPiece const hairPiece = mCharacterPieceCategories.mCollectionsByType.at( CharacterPieceType::Hair ).mPiecesById.at( params.mHairId );
	CharacterPiece const speciesPiece = mCharacterPieceCategories.mCollectionsByType.at( CharacterPieceType::Species ).mPiecesById.at( params.mSpeciesId );

	// Expect all tiles to be the same size
	// TODO: More flexibility?
	size_t const commonWidth =
		basePiece.mTileWidth &
		clothingPiece.mTileWidth &
		hairPiece.mTileWidth &
		speciesPiece.mTileWidth;
	RF_ASSERT( commonWidth == basePiece.mTileWidth );
	RF_ASSERT( commonWidth <= params.mCompositeWidth );
	size_t const commonHeight =
		basePiece.mTileHeight &
		clothingPiece.mTileHeight &
		hairPiece.mTileHeight &
		speciesPiece.mTileHeight;
	RF_ASSERT( commonHeight == basePiece.mTileHeight );
	RF_ASSERT( commonHeight <= params.mCompositeHeight );

	RF_ASSERT( basePiece.mCharacterSequenceType == CharacterSequenceType::N3_E3_S3_W3 );
	RF_ASSERT( clothingPiece.mCharacterSequenceType == CharacterSequenceType::N3_E3_S3_W3 );
	RF_ASSERT( hairPiece.mCharacterSequenceType == CharacterSequenceType::N3_E3_S3_W3 );
	RF_ASSERT( speciesPiece.mCharacterSequenceType == CharacterSequenceType::Near_far_tail_N3_E3_S3_W3 );

	file::VFSPath const& charPieces = params.mCharPiecesDir;
	file::VFSPath const& outDir = params.mOutputDir;

	CompositeAnimParams animParams = {};
	animParams.mSequence.mCharacterSequenceType = CharacterSequenceType::N3_E3_S3_W3;
	animParams.mSequence.mCompositeWidth = params.mCompositeWidth;
	animParams.mSequence.mCompositeHeight = params.mCompositeHeight;
	animParams.mSequence.mTileWidth = commonWidth;
	animParams.mSequence.mTileHeight = commonHeight;
	animParams.mSequence.mBaseCol = basePiece.mStartColumn;
	animParams.mSequence.mBaseRow = basePiece.mStartRow;
	animParams.mSequence.mClothingCol = clothingPiece.mStartColumn;
	animParams.mSequence.mClothingNearRow = clothingPiece.mStartRow;
	animParams.mSequence.mClothingFarRow = 0; // TODO: Add 'invalid' concept
	animParams.mSequence.mHairCol = hairPiece.mStartColumn;
	animParams.mSequence.mHairNearRow = hairPiece.mStartRow;
	animParams.mSequence.mHairFarRow = 0; // TODO: Add 'invalid' concept
	animParams.mSequence.mSpeciesCol = speciesPiece.mStartColumn;
	animParams.mSequence.mSpeciesNearRow = speciesPiece.mStartRow + 0;
	animParams.mSequence.mSpeciesFarRow = speciesPiece.mStartRow + 1;
	animParams.mSequence.mSpeciesTailRow = speciesPiece.mStartRow + 2;
	animParams.mBasePieces = charPieces.GetChild( basePiece.mFilename );
	animParams.mClothingPieces = charPieces.GetChild( clothingPiece.mFilename );
	animParams.mHairPieces = charPieces.GetChild( hairPiece.mFilename );
	animParams.mSpeciesPieces = charPieces.GetChild( speciesPiece.mFilename );
	animParams.mTextureOutputDirectory = outDir;
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::NWalk] = outDir.GetChild( "n.fpack" );
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::EWalk] = outDir.GetChild( "e.fpack" );
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::SWalk] = outDir.GetChild( "s.fpack" );
	animParams.mOutputPaths[CompositeAnimParams::AnimKey::WWalk] = outDir.GetChild( "w.fpack" );

	CreateCompositeAnims( animParams );
}



sprite::Bitmap CharacterCompositor::CreateCompositeFrame( CompositeFrameParams const& params )
{
	RF_ASSERT( params.mSequence.mTileWidth <= params.mSequence.mCompositeWidth );
	RF_ASSERT( params.mSequence.mTileHeight <= params.mSequence.mCompositeHeight );

	sprite::Bitmap baseFrame = params.mBaseTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mBaseCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mBaseRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap clothingNearFrame = params.mClothingTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mClothingCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mClothingNearRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap clothingFarFrame = params.mClothingTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mClothingCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mClothingFarRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap hairNearFrame = params.mHairTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mHairCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mHairNearRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap hairFarFrame = params.mHairTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mHairCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mHairFarRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesNearFrame = params.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mSpeciesCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mSpeciesNearRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesFarFrame = params.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mSpeciesCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mSpeciesFarRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	sprite::Bitmap speciesTailFrame = params.mSpeciesTex->ExtractRegion(
		params.mSequence.mTileWidth * ( params.mSequence.mSpeciesCol + params.mColumnOffset ),
		params.mSequence.mTileHeight * params.mSequence.mSpeciesTailRow,
		params.mSequence.mTileWidth,
		params.mSequence.mTileHeight );

	// Composite
	// TODO: Apply offsets
	sprite::Bitmap composite( params.mSequence.mCompositeWidth, params.mSequence.mCompositeHeight );
	{
		static constexpr sprite::Bitmap::ElementType::ElementType kMinAlpha = 1;
		composite.ApplyStencilOverwrite( speciesFarFrame, 0, 0, kMinAlpha );
		composite.ApplyStencilOverwrite( hairFarFrame, 0, 0, kMinAlpha );
		composite.ApplyStencilOverwrite( clothingFarFrame, 0, 0, kMinAlpha );
		composite.ApplyStencilOverwrite( baseFrame, 0, 0, kMinAlpha );
		composite.ApplyStencilOverwrite( clothingNearFrame, 0, 0, kMinAlpha );
		composite.ApplyStencilOverwrite( hairNearFrame, 0, 0, kMinAlpha );
		composite.ApplyStencilOverwrite( speciesTailFrame, 0, 0, kMinAlpha );
		composite.ApplyStencilOverwrite( speciesNearFrame, 0, 0, kMinAlpha );
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
	RF_ASSERT( params.mSequence.mTileWidth <= params.mSequence.mCompositeWidth );
	RF_ASSERT( params.mSequence.mTileHeight <= params.mSequence.mCompositeHeight );

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

	RF_ASSERT_MSG( params.mSequence.mCharacterSequenceType == CharacterSequenceType::N3_E3_S3_W3, "Only 'N3_E3_S3_W3' is currently supported" );

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
		frameParams.mColumnOffset = kColumnOffsets[i];
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



rftl::deque<rftl::deque<rftl::string>> CharacterCompositor::LoadCSV( file::VFSPath const& path )
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



bool CharacterCompositor::LoadPieceTable( CharacterPieceType pieceType, file::VFSPath const& pieceTablePath )
{
	rftl::deque<rftl::deque<rftl::string>> pieceTable = LoadCSV( pieceTablePath );
	if( pieceTable.empty() )
	{
		RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Failed to load piece table file" );
		return false;
	}

	CharacterPieceCollection& collection = mCharacterPieceCategories.mCollectionsByType[pieceType];

	size_t line = 0;

	rftl::deque<rftl::string> const header = pieceTable.front();
	pieceTable.pop_front();
	if(
		header.size() != 9 ||
		header.at( 0 ) != "id" ||
		header.at( 1 ) != "vfs suffix" ||
		header.at( 2 ) != "width" ||
		header.at( 3 ) != "height" ||
		header.at( 4 ) != "col" ||
		header.at( 5 ) != "row" ||
		header.at( 6 ) != "offset x" ||
		header.at( 7 ) != "offset y" ||
		header.at( 8 ) != "sequence" )
	{
		RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece table header" );
		return false;
	}
	line++;

	while( pieceTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = pieceTable.front();
		if( entry.size() != 9 )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed master piece entry at line %i, expected 9 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		rftl::string const& vfsSuffix = entry.at( 1 );
		rftl::string const& widthString = entry.at( 2 );
		rftl::string const& heightString = entry.at( 3 );
		rftl::string const& colString = entry.at( 4 );
		rftl::string const& rowString = entry.at( 5 );
		rftl::string const& offsetXString = entry.at( 6 );
		rftl::string const& offsetYString = entry.at( 7 );
		rftl::string const& sequenceString = entry.at( 8 );

		if( id.empty() )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece entry at line %i, bad id", math::integer_cast<int>( line ) );
			return false;
		}

		CharacterPiece piece = {};

		piece.mFilename = vfsSuffix;
		if( piece.mFilename.empty() )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece entry at line %i, bad filename", math::integer_cast<int>( line ) );
			return false;
		}

		( rftl::stringstream() << widthString ) >> piece.mTileWidth;
		if( piece.mTileWidth == 0 )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece entry at line %i, bad tile width", math::integer_cast<int>( line ) );
			return false;
		}

		( rftl::stringstream() << heightString ) >> piece.mTileHeight;
		if( piece.mTileHeight == 0 )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece entry at line %i, bad tile height", math::integer_cast<int>( line ) );
			return false;
		}

		( rftl::stringstream() << colString ) >> piece.mStartColumn;
		( rftl::stringstream() << rowString ) >> piece.mStartRow;
		( rftl::stringstream() << offsetXString ) >> piece.mOffsetX;
		( rftl::stringstream() << offsetYString ) >> piece.mOffsetY;

		if( sequenceString == "n3_e3_s3_w3" )
		{
			piece.mCharacterSequenceType = CharacterSequenceType::N3_E3_S3_W3;
		}
		else if( sequenceString == "near_far_tail_n3_e3_s3_w3" )
		{
			piece.mCharacterSequenceType = CharacterSequenceType::Near_far_tail_N3_E3_S3_W3;
		}
		else
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece entry at line %i, not a valid sequence type", math::integer_cast<int>( line ) );
			return false;
		}

		collection.mPiecesById[id] = rftl::move( piece );

		pieceTable.pop_front();
		line++;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}
