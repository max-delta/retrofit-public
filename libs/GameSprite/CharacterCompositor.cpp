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
#include "rftl/unordered_set"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

CharacterCompositor::CharacterCompositor( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu )
	: mVfs( vfs )
	, mPpu( ppu )
	, mBitmapCache( vfs )
{
	//
}



bool CharacterCompositor::LoadPieceTables( file::VFSPath const& pieceTablesDir )
{
	// Enumerate what modes exist
	rftl::vector<file::VFSPath> modeDirectories;
	{
		rftl::vector<file::VFSPath> unexpectedModeFiles;
		mVfs->EnumerateDirectory(
			pieceTablesDir,
			file::VFSMount::Permissions::ReadOnly,
			unexpectedModeFiles,
			modeDirectories );
		for( file::VFSPath const& unexpectedModeFile : unexpectedModeFiles )
		{
			RFLOG_WARNING( unexpectedModeFile, RFCAT_GAMESPRITE, "Unexpected file found alongside mode directories" );
		}
	}

	if( modeDirectories.empty() )
	{
		RFLOG_NOTIFY( pieceTablesDir, RFCAT_GAMESPRITE, "Failed to find any mode directories" );
		return false;
	}

	for( file::VFSPath const& modeDirectory : modeDirectories )
	{
		rftl::string const mode = modeDirectory.GetElement( modeDirectory.NumElements() - 1 );

		{
			CharacterPieceType type = CharacterPieceType::Base;
			file::VFSPath const pieceTableDirectory = modeDirectory.GetChild( "base" );
			bool const loadResult = LoadPieceTableDirectory( mode, type, pieceTableDirectory );
			if( loadResult == false )
			{
				RFLOG_NOTIFY( pieceTableDirectory, RFCAT_GAMESPRITE, "Couldn't load piece table directory" );
				return false;
			}
		}
		{
			CharacterPieceType type = CharacterPieceType::Top;
			file::VFSPath const pieceTableDirectory = modeDirectory.GetChild( "top" );
			bool const loadResult = LoadPieceTableDirectory( mode, type, pieceTableDirectory );
			if( loadResult == false )
			{
				RFLOG_NOTIFY( pieceTableDirectory, RFCAT_GAMESPRITE, "Couldn't load piece table directory" );
				return false;
			}
		}
		{
			CharacterPieceType type = CharacterPieceType::Bottom;
			file::VFSPath const pieceTableDirectory = modeDirectory.GetChild( "bottom" );
			bool const loadResult = LoadPieceTableDirectory( mode, type, pieceTableDirectory );
			if( loadResult == false )
			{
				RFLOG_NOTIFY( pieceTableDirectory, RFCAT_GAMESPRITE, "Couldn't load piece table directory" );
				return false;
			}
		}
		{
			CharacterPieceType type = CharacterPieceType::Hair;
			file::VFSPath const pieceTableDirectory = modeDirectory.GetChild( "hair" );
			bool const loadResult = LoadPieceTableDirectory( mode, type, pieceTableDirectory );
			if( loadResult == false )
			{
				RFLOG_NOTIFY( pieceTableDirectory, RFCAT_GAMESPRITE, "Couldn't load piece table directory" );
				return false;
			}
		}
		{
			CharacterPieceType type = CharacterPieceType::Species;
			file::VFSPath const pieceTableDirectory = modeDirectory.GetChild( "species" );
			bool const loadResult = LoadPieceTableDirectory( mode, type, pieceTableDirectory );
			if( loadResult == false )
			{
				RFLOG_NOTIFY( pieceTableDirectory, RFCAT_GAMESPRITE, "Couldn't load piece table directory" );
				return false;
			}
		}
	}

	return true;
}



CompositeCharacter CharacterCompositor::CreateCompositeCharacter( CompositeCharacterParams const& params )
{
	// TODO: Better error-reporting

	CharacterPieceCategories& categories = mCharacterModes.mCategoriesByMode.at( params.mMode );

	CharacterPiece const basePiece = categories.mCollectionsByType.at( CharacterPieceType::Base ).mPiecesById.at( params.mBaseId );
	CharacterPiece const topPiece = categories.mCollectionsByType.at( CharacterPieceType::Top ).mPiecesById.at( params.mTopId );
	CharacterPiece const bottomPiece = categories.mCollectionsByType.at( CharacterPieceType::Bottom ).mPiecesById.at( params.mBottomId );
	CharacterPiece const hairPiece = categories.mCollectionsByType.at( CharacterPieceType::Hair ).mPiecesById.at( params.mHairId );
	CharacterPiece const speciesPiece = categories.mCollectionsByType.at( CharacterPieceType::Species ).mPiecesById.at( params.mSpeciesId );

	// Expect all tiles to be the same size
	// TODO: More flexibility?
	size_t const commonWidth =
		basePiece.mTileWidth &
		topPiece.mTileWidth &
		bottomPiece.mTileWidth &
		hairPiece.mTileWidth &
		speciesPiece.mTileWidth;
	RF_ASSERT( commonWidth == basePiece.mTileWidth );
	RF_ASSERT( commonWidth <= params.mCompositeWidth );
	size_t const commonHeight =
		basePiece.mTileHeight &
		topPiece.mTileHeight &
		bottomPiece.mTileHeight &
		hairPiece.mTileHeight &
		speciesPiece.mTileHeight;
	RF_ASSERT( commonHeight == basePiece.mTileHeight );
	RF_ASSERT( commonHeight <= params.mCompositeHeight );

	RF_ASSERT( basePiece.mCharacterSequenceType == CharacterSequenceType::P_NESW_i121 );
	RF_ASSERT( topPiece.mCharacterSequenceType == CharacterSequenceType::P_NESW_i121 );
	RF_ASSERT( bottomPiece.mCharacterSequenceType == CharacterSequenceType::P_NESW_i121 );
	RF_ASSERT( hairPiece.mCharacterSequenceType == CharacterSequenceType::P_NESW_i121 );
	RF_ASSERT( speciesPiece.mCharacterSequenceType == CharacterSequenceType::Far_mid_near_P_NESW_i121 );

	file::VFSPath const& charPieces = params.mCharPiecesDir;
	file::VFSPath const& outDir = params.mOutputDir;

	CompositeAnimParams animParams = {};
	animParams.mSequence.mCharacterSequenceType = CharacterSequenceType::P_NESW_i121;
	animParams.mSequence.mCompositeWidth = params.mCompositeWidth;
	animParams.mSequence.mCompositeHeight = params.mCompositeHeight;
	animParams.mSequence.mTileWidth = commonWidth;
	animParams.mSequence.mTileHeight = commonHeight;
	animParams.mSequence.mBaseCol = basePiece.mStartColumn;
	animParams.mSequence.mBaseRow = basePiece.mStartRow;
	animParams.mSequence.mBaseOffsetX = basePiece.mOffsetX;
	animParams.mSequence.mBaseOffsetY = basePiece.mOffsetY;
	animParams.mSequence.mTopCol = topPiece.mStartColumn;
	animParams.mSequence.mTopFarRow = CompositeSequenceParams::kInvalidRow;
	animParams.mSequence.mTopNearRow = topPiece.mStartRow;
	animParams.mSequence.mTopOffsetX = topPiece.mOffsetX;
	animParams.mSequence.mTopOffsetY = topPiece.mOffsetY;
	animParams.mSequence.mBottomCol = topPiece.mStartColumn;
	animParams.mSequence.mBottomFarRow = CompositeSequenceParams::kInvalidRow;
	animParams.mSequence.mBottomNearRow = bottomPiece.mStartRow;
	animParams.mSequence.mBottomOffsetX = bottomPiece.mOffsetX;
	animParams.mSequence.mBottomOffsetY = bottomPiece.mOffsetY;
	animParams.mSequence.mHairCol = hairPiece.mStartColumn;
	animParams.mSequence.mHairFarRow = CompositeSequenceParams::kInvalidRow;
	animParams.mSequence.mHairNearRow = hairPiece.mStartRow;
	animParams.mSequence.mHairOffsetX = hairPiece.mOffsetX;
	animParams.mSequence.mHairOffsetY = hairPiece.mOffsetY;
	animParams.mSequence.mSpeciesCol = speciesPiece.mStartColumn;
	animParams.mSequence.mSpeciesFarRow = speciesPiece.mStartRow + 0;
	animParams.mSequence.mSpeciesMidRow = speciesPiece.mStartRow + 1;
	animParams.mSequence.mSpeciesNearRow = speciesPiece.mStartRow + 2;
	animParams.mSequence.mSpeciesOffsetX = speciesPiece.mOffsetX;
	animParams.mSequence.mSpeciesOffsetY = speciesPiece.mOffsetY;
	animParams.mBasePieces = charPieces.GetChild( params.mMode, "base", basePiece.mFilename );
	animParams.mTopPieces = charPieces.GetChild( params.mMode, "top", topPiece.mFilename );
	animParams.mBottomPieces = charPieces.GetChild( params.mMode, "bottom", bottomPiece.mFilename );
	animParams.mHairPieces = charPieces.GetChild( params.mMode, "hair", hairPiece.mFilename );
	animParams.mSpeciesPieces = charPieces.GetChild( params.mMode, "species", speciesPiece.mFilename );
	animParams.mTextureOutputDirectory = outDir;
	animParams.mOutputPaths[CharacterAnimKey::NIdle] = outDir.GetChild( "n_idle.fpack" );
	animParams.mOutputPaths[CharacterAnimKey::EIdle] = outDir.GetChild( "e_idle.fpack" );
	animParams.mOutputPaths[CharacterAnimKey::SIdle] = outDir.GetChild( "s_idle.fpack" );
	animParams.mOutputPaths[CharacterAnimKey::WIdle] = outDir.GetChild( "w_idle.fpack" );
	animParams.mOutputPaths[CharacterAnimKey::NWalk] = outDir.GetChild( "n_walk.fpack" );
	animParams.mOutputPaths[CharacterAnimKey::EWalk] = outDir.GetChild( "e_walk.fpack" );
	animParams.mOutputPaths[CharacterAnimKey::SWalk] = outDir.GetChild( "s_walk.fpack" );
	animParams.mOutputPaths[CharacterAnimKey::WWalk] = outDir.GetChild( "w_walk.fpack" );

	CreateCompositeAnims( animParams );

	CompositeCharacter retVal = {};
	retVal.mCharacterSequenceType = animParams.mSequence.mCharacterSequenceType;
	retVal.mFramepacksByAnim = rftl::move( animParams.mOutputPaths );
	return retVal;
}



sprite::Bitmap CharacterCompositor::CreateCompositeFrame( CompositeFrameParams const& params )
{
	CompositeSequenceParams const& sequence = params.mSequence;

	RF_ASSERT( sequence.mTileWidth <= sequence.mCompositeWidth );
	RF_ASSERT( sequence.mTileHeight <= sequence.mCompositeHeight );

	// Composite
	sprite::Bitmap composite( sequence.mCompositeWidth, sequence.mCompositeHeight );
	{
		static constexpr sprite::Bitmap::ElementType::ElementType kMinAlpha = 1;

		struct Input
		{
			size_t const row;
			size_t const col;
			size_t const offsetX;
			size_t const offsetY;
			sprite::Bitmap const* const tex;

			RF_ACK_AGGREGATE_NOCOPY();
		};
		Input const input[] = {
			{ sequence.mSpeciesFarRow, sequence.mSpeciesCol, sequence.mSpeciesOffsetX, sequence.mSpeciesOffsetY, params.mSpeciesTex },
			{ sequence.mHairFarRow, sequence.mHairCol, sequence.mHairOffsetX, sequence.mHairOffsetY, params.mHairTex },
			{ sequence.mTopFarRow, sequence.mTopCol, sequence.mTopOffsetX, sequence.mTopOffsetY, params.mTopTex },
			{ sequence.mBottomFarRow, sequence.mBottomCol, sequence.mBottomOffsetX, sequence.mBottomOffsetY, params.mBottomTex },
			{ sequence.mBaseRow, sequence.mBaseCol, sequence.mBaseOffsetX, sequence.mBaseOffsetY, params.mBaseTex },
			{ sequence.mSpeciesMidRow, sequence.mSpeciesCol, sequence.mSpeciesOffsetX, sequence.mSpeciesOffsetY, params.mSpeciesTex },
			{ sequence.mBottomNearRow, sequence.mBottomCol, sequence.mBottomOffsetX, sequence.mBottomOffsetY, params.mBottomTex },
			{ sequence.mTopNearRow, sequence.mTopCol, sequence.mTopOffsetX, sequence.mTopOffsetY, params.mTopTex },
			{ sequence.mHairNearRow, sequence.mHairCol, sequence.mHairOffsetX, sequence.mHairOffsetY, params.mHairTex },
			{ sequence.mSpeciesNearRow, sequence.mSpeciesCol, sequence.mSpeciesOffsetX, sequence.mSpeciesOffsetY, params.mSpeciesTex }
		};

		for( Input const& entry : input )
		{
			if( entry.row != CompositeSequenceParams::kInvalidRow )
			{
				sprite::Bitmap frame = entry.tex->ExtractRegion(
					sequence.mTileWidth * ( entry.col + params.mColumnOffset ),
					sequence.mTileHeight * entry.row,
					sequence.mTileWidth,
					sequence.mTileHeight );
				composite.ApplyStencilOverwrite( frame, entry.offsetX, entry.offsetY, kMinAlpha );
			}
		}
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
	WeakPtr<sprite::Bitmap const> topTex = mBitmapCache.Fetch( params.mTopPieces );
	WeakPtr<sprite::Bitmap const> bottomTex = mBitmapCache.Fetch( params.mBottomPieces );
	WeakPtr<sprite::Bitmap const> hairTex = mBitmapCache.Fetch( params.mHairPieces );
	WeakPtr<sprite::Bitmap const> speciesTex = mBitmapCache.Fetch( params.mSpeciesPieces );

	CompositeFrameParams frameParams = {};
	frameParams.mSequence = params.mSequence;
	frameParams.mBaseTex = baseTex;
	frameParams.mTopTex = topTex;
	frameParams.mBottomTex = bottomTex;
	frameParams.mHairTex = hairTex;
	frameParams.mSpeciesTex = speciesTex;

	RF_ASSERT_MSG( params.mSequence.mCharacterSequenceType == CharacterSequenceType::P_NESW_i121, "Only 'P_NESW_i121' is currently supported" );

	static constexpr size_t kBitmapFramesPerDirection = 4;
	static constexpr size_t kTotalFrames = 4 * kBitmapFramesPerDirection;
	static constexpr char const* kFrameNames[kTotalFrames] = {
		// clang-format off
		"ni.bmp", "n0.bmp", "n1.bmp", "n2.bmp",
		"ei.bmp", "e0.bmp", "e1.bmp", "e2.bmp",
		"si.bmp", "s0.bmp", "s1.bmp", "s2.bmp",
		"wi.bmp", "w0.bmp", "w1.bmp", "w2.bmp"
		// clang-format on
	};
	static constexpr size_t kColumnOffsets[kTotalFrames] = {
		// clang-format off
		1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10, 11, 12,
		13, 14, 15, 16
		// clang-format on
	};

	// Write frames to disk
	for( size_t i = 0; i < kTotalFrames; i++ )
	{
		frameParams.mColumnOffset = kColumnOffsets[i];
		WriteFrameToDisk( CreateCompositeFrame( frameParams ), params.mTextureOutputDirectory.GetChild( kFrameNames[i] ) );
	}

	static constexpr size_t kAnimFramesPerDirection = 4;
	static constexpr size_t kTotalFramepacks = 8;
	static constexpr CharacterAnimKey kFramepackKeys[kTotalFramepacks] = {
		CharacterAnimKey::NIdle,
		CharacterAnimKey::EIdle,
		CharacterAnimKey::SIdle,
		CharacterAnimKey::WIdle,
		CharacterAnimKey::NWalk,
		CharacterAnimKey::EWalk,
		CharacterAnimKey::SWalk,
		CharacterAnimKey::WWalk
	};
	static constexpr size_t kFramepackSourceFrames[kTotalFramepacks][kAnimFramesPerDirection] = {
		{ 0, 0, 0, 0 },
		{ 4, 4, 4, 4 },
		{ 8, 8, 8, 8 },
		{ 12, 12, 12, 12 },
		{ 1, 2, 3, 2 },
		{ 5, 6, 7, 6 },
		{ 9, 10, 11, 10 },
		{ 13, 14, 15, 14 }
	};

	// Create framepacks
	for( size_t i_framepack = 0; i_framepack < kTotalFramepacks; i_framepack++ )
	{
		size_t const( &sourceFrames )[kAnimFramesPerDirection] = kFramepackSourceFrames[i_framepack];
		CharacterAnimKey const& framepackKey = kFramepackKeys[i_framepack];
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
		rftl::unordered_set<rftl::string> frameResourceNames;
		for( size_t i = 0; i < kAnimFramesPerDirection; i++ )
		{
			file::VFSPath const frameTexture = params.mTextureOutputDirectory.GetChild( kFrameNames[sourceFrames[i]] );
			frameResourceNames.emplace( frameTexture.CreateString() );
		}
		for( rftl::string const& resourceName : frameResourceNames )
		{
			texMan.DestroyResource( resourceName );
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
	if( buffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_GAMESPRITE, "Failed to get data from file buffer" );
		return {};
	}

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( buffer.GetChars() );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_GAMESPRITE, "Failed to read file as csv" );
		return {};
	}

	return csv;
}



bool CharacterCompositor::LoadPieceTable( rftl::string mode, CharacterPieceType pieceType, file::VFSPath const& pieceTablePath )
{
	rftl::deque<rftl::deque<rftl::string>> pieceTable = LoadCSV( pieceTablePath );
	if( pieceTable.empty() )
	{
		RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Failed to load piece table file" );
		return false;
	}

	CharacterPieceCategories& categories = mCharacterModes.mCategoriesByMode[mode];
	CharacterPieceCollection& collection = categories.mCollectionsByType[pieceType];

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

		if( sequenceString == "p_nesw_i121" )
		{
			piece.mCharacterSequenceType = CharacterSequenceType::P_NESW_i121;
		}
		else if( sequenceString == "fmn_p_nesw_i121" )
		{
			piece.mCharacterSequenceType = CharacterSequenceType::Far_mid_near_P_NESW_i121;
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



bool CharacterCompositor::LoadPieceTableDirectory( rftl::string mode, CharacterPieceType pieceType, file::VFSPath const& pieceTableDirectory )
{
	// Enumerate piece tables
	rftl::vector<file::VFSPath> pieceTableFiles;
	{
		rftl::vector<file::VFSPath> unexpectedDirectories;
		mVfs->EnumerateDirectory(
			pieceTableDirectory,
			file::VFSMount::Permissions::ReadOnly,
			pieceTableFiles,
			unexpectedDirectories );
		for( file::VFSPath const& unexpectedDirectory : unexpectedDirectories )
		{
			RFLOG_WARNING( unexpectedDirectory, RFCAT_GAMESPRITE, "Unexpected directory found alongside piece table files" );
		}
	}

	if( pieceTableFiles.empty() )
	{
		RFLOG_NOTIFY( pieceTableDirectory, RFCAT_GAMESPRITE, "Failed to find any piece table files" );
		return false;
	}

	// Load all piece tables
	for( file::VFSPath const& pieceTableFile : pieceTableFiles )
	{
		bool const loadResult = LoadPieceTable( mode, pieceType, pieceTableFile );
		if( loadResult == false )
		{
			RFLOG_NOTIFY( pieceTableFile, RFCAT_GAMESPRITE, "Couldn't load piece table file" );
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
