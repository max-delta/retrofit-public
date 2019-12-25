#pragma once
#include "project.h"

#include "GameSprite/CompositeCharacter.h"
#include "GameSprite/BitmapCache.h"

#include "PPU/PPUFwd.h"

#include "rftl/deque"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

struct GAMESPRITE_API CompositeCharacterParams
{
	size_t mCompositeWidth = 0;
	size_t mCompositeHeight = 0;

	rftl::string mBaseId = {};
	rftl::string mClothingId = {};
	rftl::string mHairId = {};
	rftl::string mSpeciesId = {};

	file::VFSPath mCharPiecesDir = {};
	file::VFSPath mOutputDir = {};
};

///////////////////////////////////////////////////////////////////////////////

enum class CharacterPieceType : uint8_t
{
	Invalid = 0,
	Base,
	Clothing,
	Hair,
	Species
};



struct GAMESPRITE_API CompositeSequenceParams
{
	CharacterSequenceType mCharacterSequenceType = CharacterSequenceType::Invalid;

	size_t mCompositeWidth = 0;
	size_t mCompositeHeight = 0;

	size_t mTileWidth = 0;
	size_t mTileHeight = 0;

	size_t mBaseCol = 0;
	size_t mBaseRow = 0;
	size_t mBaseOffsetX = 0;
	size_t mBaseOffsetY = 0;

	size_t mClothingCol = 0;
	size_t mClothingNearRow = 0;
	size_t mClothingFarRow = 0;
	size_t mClothingOffsetX = 0;
	size_t mClothingOffsetY = 0;

	size_t mHairCol = 0;
	size_t mHairNearRow = 0;
	size_t mHairFarRow = 0;
	size_t mHairOffsetX = 0;
	size_t mHairOffsetY = 0;

	size_t mSpeciesCol = 0;
	size_t mSpeciesNearRow = 0;
	size_t mSpeciesFarRow = 0;
	size_t mSpeciesTailRow = 0;
	size_t mSpeciesOffsetX = 0;
	size_t mSpeciesOffsetY = 0;
};



struct GAMESPRITE_API CompositeFrameParams
{
	CompositeSequenceParams mSequence = {};

	size_t mColumnOffset = 0;

	sprite::Bitmap const* mBaseTex = nullptr;
	sprite::Bitmap const* mClothingTex = nullptr;
	sprite::Bitmap const* mHairTex = nullptr;
	sprite::Bitmap const* mSpeciesTex = nullptr;
};



struct GAMESPRITE_API CompositeAnimParams
{
	CompositeSequenceParams mSequence = {};

	file::VFSPath mBasePieces = {};
	file::VFSPath mClothingPieces = {};
	file::VFSPath mHairPieces = {};
	file::VFSPath mSpeciesPieces = {};

	file::VFSPath mTextureOutputDirectory = {};

	rftl::unordered_map<CharacterAnimKey, file::VFSPath> mOutputPaths = {};
};



struct GAMESPRITE_API CharacterPiece
{
	rftl::string mFilename = {};
	size_t mTileWidth = 0;
	size_t mTileHeight = 0;
	size_t mStartColumn = 0;
	size_t mStartRow = 0;
	size_t mOffsetX = 0;
	size_t mOffsetY = 0;
	CharacterSequenceType mCharacterSequenceType = CharacterSequenceType::Invalid;
};



struct GAMESPRITE_API CharacterPieceCollection
{
	using PiecesById = rftl::unordered_map<rftl::string, CharacterPiece>;
	PiecesById mPiecesById = {};
};



struct GAMESPRITE_API CharacterPieceCategories
{
	using CollectionsByType = rftl::unordered_map<CharacterPieceType, CharacterPieceCollection>;
	CollectionsByType mCollectionsByType = {};
};

///////////////////////////////////////////////////////////////////////////////

class GAMESPRITE_API CharacterCompositor
{
	RF_NO_COPY( CharacterCompositor );

	//
	// Public methods
public:
	CharacterCompositor( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::PPUController> ppu );

	bool LoadPieceTables( file::VFSPath const& masterTablePath, file::VFSPath const& pieceTablesDir );
	CompositeCharacter CreateCompositeCharacter( CompositeCharacterParams const& params );


	//
	// Private methods
private:
	static sprite::Bitmap CreateCompositeFrame( CompositeFrameParams const& params );
	void WriteFrameToDisk( sprite::Bitmap const& frame, file::VFSPath const& path );
	void CreateCompositeAnims( CompositeAnimParams const& params );

	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );
	bool LoadPieceTable( CharacterPieceType pieceType, file::VFSPath const& pieceTablePath );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	WeakPtr<gfx::PPUController> mPpu;
	BitmapCache mBitmapCache;
	CharacterPieceCategories mCharacterPieceCategories;
};

///////////////////////////////////////////////////////////////////////////////
}}
