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

	file::VFSPath const& charPieces = params.mCharPiecesDir;
	file::VFSPath const& outDir = params.mOutputDir;

	CompositeAnimParams animParams = {};
	animParams.mSequence.mCompositeWidth = params.mCompositeWidth;
	animParams.mSequence.mCompositeHeight = params.mCompositeHeight;
	animParams.mBasePieces = charPieces.GetChild( params.mMode, "base", basePiece.mResources );
	animParams.mTopPieces = charPieces.GetChild( params.mMode, "top", topPiece.mResources );
	animParams.mBottomPieces = charPieces.GetChild( params.mMode, "bottom", bottomPiece.mResources );
	animParams.mHairPieces = charPieces.GetChild( params.mMode, "hair", hairPiece.mResources );
	animParams.mSpeciesPieces = charPieces.GetChild( params.mMode, "species", speciesPiece.mResources );
	animParams.mTextureOutputDirectory = outDir;
	animParams.mOutputPaths["idle_n"] = outDir.GetChild( "idle_n.fpack" );
	animParams.mOutputPaths["idle_e"] = outDir.GetChild( "idle_e.fpack" );
	animParams.mOutputPaths["idle_s"] = outDir.GetChild( "idle_s.fpack" );
	animParams.mOutputPaths["idle_w"] = outDir.GetChild( "idle_w.fpack" );
	animParams.mOutputPaths["walk_n"] = outDir.GetChild( "walk_n.fpack" );
	animParams.mOutputPaths["walk_e"] = outDir.GetChild( "walk_e.fpack" );
	animParams.mOutputPaths["walk_s"] = outDir.GetChild( "walk_s.fpack" );
	animParams.mOutputPaths["walk_w"] = outDir.GetChild( "walk_w.fpack" );

	CreateCompositeAnims( animParams );

	CompositeCharacter retVal = {};
	retVal.mFramepacksByAnim = rftl::move( animParams.mOutputPaths );
	return retVal;
}



sprite::Bitmap CharacterCompositor::CreateCompositeFrame( CompositeFrameParams const& params )
{
	CompositeSequenceParams const& sequence = params.mSequence;

	RF_ASSERT( params.mTileWidth <= sequence.mCompositeWidth );
	RF_ASSERT( params.mTileHeight <= sequence.mCompositeHeight );

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
			{ params.mSpeciesFarRow, params.mSpeciesCol, params.mSpeciesOffsetX, params.mSpeciesOffsetY, params.mSpeciesTex },
			{ params.mHairFarRow, params.mHairCol, params.mHairOffsetX, params.mHairOffsetY, params.mHairTex },
			{ params.mTopFarRow, params.mTopCol, params.mTopOffsetX, params.mTopOffsetY, params.mTopTex },
			{ params.mBottomFarRow, params.mBottomCol, params.mBottomOffsetX, params.mBottomOffsetY, params.mBottomTex },
			{ params.mBaseRow, params.mBaseCol, params.mBaseOffsetX, params.mBaseOffsetY, params.mBaseTex },
			{ params.mSpeciesMidRow, params.mSpeciesCol, params.mSpeciesOffsetX, params.mSpeciesOffsetY, params.mSpeciesTex },
			{ params.mBottomNearRow, params.mBottomCol, params.mBottomOffsetX, params.mBottomOffsetY, params.mBottomTex },
			{ params.mTopNearRow, params.mTopCol, params.mTopOffsetX, params.mTopOffsetY, params.mTopTex },
			{ params.mHairNearRow, params.mHairCol, params.mHairOffsetX, params.mHairOffsetY, params.mHairTex },
			{ params.mSpeciesNearRow, params.mSpeciesCol, params.mSpeciesOffsetX, params.mSpeciesOffsetY, params.mSpeciesTex } };

		for( Input const& entry : input )
		{
			if(
				entry.row != CompositeFrameParams::kInvalidRow &&
				entry.col != CompositeFrameParams::kInvalidCol )
			{
				sprite::Bitmap frame = entry.tex->ExtractRegion(
					params.mTileWidth * entry.col,
					params.mTileHeight * entry.row,
					params.mTileWidth,
					params.mTileHeight );
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
	for( CompositeAnimParams::OutputPaths::value_type const& outputPath : params.mOutputPaths )
	{
		CreateCompositeAnim( params, outputPath.first );
	}
}



void CharacterCompositor::CreateCompositeAnim( CompositeAnimParams const& params, CharacterAnimKey const& anim )
{
	file::VFSPath const& framepackPath = params.mOutputPaths.at( anim );

	// HACK: Assume PNG
	// TODO: Check for presence of other types?
	rftl::string const animFilename = anim + ".png";

	// Load textures
	WeakPtr<sprite::Bitmap const> baseTex = mBitmapCache.Fetch( params.mBasePieces.GetChild( animFilename ) );
	WeakPtr<sprite::Bitmap const> topTex = mBitmapCache.Fetch( params.mTopPieces.GetChild( animFilename ) );
	WeakPtr<sprite::Bitmap const> bottomTex = mBitmapCache.Fetch( params.mBottomPieces.GetChild( animFilename ) );
	WeakPtr<sprite::Bitmap const> hairTex = mBitmapCache.Fetch( params.mHairPieces.GetChild( animFilename ) );
	WeakPtr<sprite::Bitmap const> speciesTex = mBitmapCache.Fetch( params.mSpeciesPieces.GetChild( animFilename ) );

	// Use base to determine shaping
	// TODO: More flexibility?
	size_t const tileHeight = baseTex->GetHeight();
	RF_ASSERT( tileHeight <= params.mSequence.mCompositeHeight );
	size_t const tileWidth = tileHeight;
	RF_ASSERT( tileWidth <= params.mSequence.mCompositeWidth );
	RF_ASSERT( baseTex->GetWidth() % tileWidth == 0 );
	size_t const numUniqueFrames = baseTex->GetWidth() / tileWidth;

	// Make sure other pieces match
	// TODO: More flexibility?
	RF_ASSERT( topTex->GetHeight() == tileHeight * 1 );
	RF_ASSERT( bottomTex->GetHeight() == tileHeight * 1 );
	RF_ASSERT( hairTex->GetHeight() == tileHeight * 1 );
	RF_ASSERT( speciesTex->GetHeight() == tileHeight * 3 );
	RF_ASSERT( topTex->GetWidth() == tileWidth * numUniqueFrames );
	RF_ASSERT( bottomTex->GetWidth() == tileWidth * numUniqueFrames );
	RF_ASSERT( hairTex->GetWidth() == tileWidth * numUniqueFrames );
	RF_ASSERT( speciesTex->GetWidth() == tileWidth * numUniqueFrames );

	// Setup common frame params
	CompositeFrameParams frameParams = {};
	{
		frameParams.mSequence = params.mSequence;
		frameParams.mBaseTex = baseTex;
		frameParams.mTopTex = topTex;
		frameParams.mBottomTex = bottomTex;
		frameParams.mHairTex = hairTex;
		frameParams.mSpeciesTex = speciesTex;

		frameParams.mTileWidth = tileWidth;
		frameParams.mTileHeight = tileHeight;

		static constexpr size_t kCommonOffsetX = 0;

		// HACK: Special case for awokenh_f
		// TODO: Fix data
		size_t commonOffsetY = 0;
		if( tileHeight == 36 )
		{
			commonOffsetY = 4;
		}

		frameParams.mBaseRow = 0;
		frameParams.mBaseOffsetX = kCommonOffsetX;
		frameParams.mBaseOffsetY = commonOffsetY;
		frameParams.mTopFarRow = CompositeFrameParams::kInvalidRow;
		frameParams.mTopNearRow = 0;
		frameParams.mTopOffsetX = kCommonOffsetX;
		frameParams.mTopOffsetY = commonOffsetY;
		frameParams.mBottomFarRow = CompositeFrameParams::kInvalidRow;
		frameParams.mBottomNearRow = 0;
		frameParams.mBottomOffsetX = kCommonOffsetX;
		frameParams.mBottomOffsetY = commonOffsetY;
		frameParams.mHairFarRow = CompositeFrameParams::kInvalidRow;
		frameParams.mHairNearRow = 0;
		frameParams.mHairOffsetX = kCommonOffsetX;
		frameParams.mHairOffsetY = commonOffsetY;
		frameParams.mSpeciesFarRow = 0;
		frameParams.mSpeciesMidRow = 1;
		frameParams.mSpeciesNearRow = 2;
		frameParams.mSpeciesOffsetX = kCommonOffsetX;
		frameParams.mSpeciesOffsetY = commonOffsetY;
	}

	// HACK: Special case for awokenh_f
	// TODO: Fix data
	if( tileHeight == 36 )
	{
		if( params.mSpeciesPieces.GetLastElement() == "awokenh_f" )
		{
			frameParams.mSpeciesOffsetY = 0;
		}
	}

	auto const calcFrameName = [&anim]( size_t frame ) -> rftl::string //
	{
		return anim + '_' + rftl::to_string( frame ) + ".bmp";
	};

	// Write frames to disk
	for( size_t i = 0; i < numUniqueFrames; i++ )
	{
		rftl::string const frameName = calcFrameName( i );

		// TODO: Some sort of elaborate scheme for inconsistent frame counts or
		//  other fancy ways to cut down on texture counts
		frameParams.mBaseCol = i;
		frameParams.mTopCol = i;
		frameParams.mBottomCol = i;
		frameParams.mHairCol = i;
		frameParams.mSpeciesCol = i;

		WriteFrameToDisk( CreateCompositeFrame( frameParams ), params.mTextureOutputDirectory.GetChild( frameName ) );
	}

	// Determine unique frames -> source frames (some frames may be re-used)
	// HACK: Hard-coded
	// TODO: Use data to control this
	rftl::vector<size_t> sourceFrames;
	for( size_t i = 0; i < numUniqueFrames; i++ )
	{
		sourceFrames.emplace_back( i );
	}
	if(
		anim == "walk_n" ||
		anim == "walk_e" ||
		anim == "walk_s" ||
		anim == "walk_w" )
	{
		RF_ASSERT( numUniqueFrames == 3 );
		sourceFrames = { 0, 1, 2, 1 };
	}

	// HACK: Hard-coded
	// TODO: Use data to control this
	static constexpr uint8_t kHACKSlowdownRate = 6;
	uint8_t const originX = math::integer_cast<uint8_t>( tileWidth / 2 );
	uint8_t const originY = math::integer_cast<uint8_t>( tileHeight * 5 / 6 );

	// HACK: Direct access to texture manager
	// TODO: Re-visit API surface
	gfx::TextureManager& texMan = *mPpu->DebugGetTextureManager();

	// Create framepack
	gfx::FramePack_256 newFPack = {};
	newFPack.mNumTimeSlots = math::integer_cast<uint8_t>( sourceFrames.size() );
	newFPack.mPreferredSlowdownRate = kHACKSlowdownRate;
	for( size_t i = 0; i < sourceFrames.size(); i++ )
	{
		rftl::string const frameName = calcFrameName( i );
		file::VFSPath const frameTexture = params.mTextureOutputDirectory.GetChild( calcFrameName( sourceFrames.at( i ) ) );
		gfx::FramePackBase::TimeSlot& slot = newFPack.GetMutableTimeSlots()[i];
		slot.mTextureReference = texMan.LoadNewResourceGetID( frameTexture );
		slot.mTextureOriginX = originX;
		slot.mTextureOriginY = originY;
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
	for( size_t i = 0; i < numUniqueFrames; i++ )
	{
		file::VFSPath const frameTexture = params.mTextureOutputDirectory.GetChild( calcFrameName( sourceFrames[i] ) );
		frameResourceNames.emplace( frameTexture.CreateString() );
	}
	for( rftl::string const& resourceName : frameResourceNames )
	{
		texMan.DestroyResource( resourceName );
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
		header.size() != 2 ||
		header.at( 0 ) != "id" ||
		header.at( 1 ) != "resources" )
	{
		RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece table header" );
		return false;
	}
	line++;

	while( pieceTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = pieceTable.front();
		if( entry.size() != 2 )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed master piece entry at line %i, expected 2 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		rftl::string const& resources = entry.at( 1 );

		if( id.empty() )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece entry at line %i, bad id", math::integer_cast<int>( line ) );
			return false;
		}

		CharacterPiece piece = {};

		piece.mResources = resources;
		if( piece.mResources.empty() )
		{
			RFLOG_NOTIFY( pieceTablePath, RFCAT_GAMESPRITE, "Malformed piece entry at line %i, bad resources", math::integer_cast<int>( line ) );
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
