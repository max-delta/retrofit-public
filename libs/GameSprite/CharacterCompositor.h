#pragma once
#include "project.h"

#include "GameSprite/CompositeCharacter.h"
#include "GameSprite/BitmapCache.h"

#include "PPU/PPUFwd.h"

#include "rftl/deque"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

struct GAMESPRITE_API CompositeCharacterParams
{
	rftl::string mMode = {};

	size_t mCompositeWidth = 0;
	size_t mCompositeHeight = 0;

	rftl::string mBaseId = {};
	rftl::string mTopId = {};
	rftl::string mBottomId = {};
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
	Top,
	Bottom,
	Hair,
	Species
};



struct GAMESPRITE_API CompositeSequenceParams
{
	size_t mCompositeWidth = 0;
	size_t mCompositeHeight = 0;
};



struct GAMESPRITE_API CompositeFrameParams
{
	static constexpr size_t kInvalidRow = rftl::numeric_limits<size_t>::max();
	static constexpr size_t kInvalidCol = rftl::numeric_limits<size_t>::max();

	CompositeSequenceParams mSequence = {};

	size_t mTileWidth = 0;
	size_t mTileHeight = 0;

	size_t mBaseCol = 0;
	size_t mBaseRow = 0;
	size_t mBaseOffsetX = 0;
	size_t mBaseOffsetY = 0;

	size_t mTopCol = 0;
	size_t mTopFarRow = 0;
	size_t mTopNearRow = 0;
	size_t mTopOffsetX = 0;
	size_t mTopOffsetY = 0;

	size_t mBottomCol = 0;
	size_t mBottomFarRow = 0;
	size_t mBottomNearRow = 0;
	size_t mBottomOffsetX = 0;
	size_t mBottomOffsetY = 0;

	size_t mHairCol = 0;
	size_t mHairFarRow = 0;
	size_t mHairNearRow = 0;
	size_t mHairOffsetX = 0;
	size_t mHairOffsetY = 0;

	size_t mSpeciesCol = 0;
	size_t mSpeciesFarRow = 0;
	size_t mSpeciesMidRow = 0;
	size_t mSpeciesNearRow = 0;
	size_t mSpeciesOffsetX = 0;
	size_t mSpeciesOffsetY = 0;

	sprite::Bitmap const* mBaseTex = nullptr;
	sprite::Bitmap const* mTopTex = nullptr;
	sprite::Bitmap const* mBottomTex = nullptr;
	sprite::Bitmap const* mHairTex = nullptr;
	sprite::Bitmap const* mSpeciesTex = nullptr;
};



struct GAMESPRITE_API CompositeAnimParams
{
	CompositeSequenceParams mSequence = {};

	file::VFSPath mBasePieces = {};
	file::VFSPath mTopPieces = {};
	file::VFSPath mBottomPieces = {};
	file::VFSPath mHairPieces = {};
	file::VFSPath mSpeciesPieces = {};

	file::VFSPath mTextureOutputDirectory = {};

	using OutputPaths = rftl::unordered_map<CharacterAnimKey, file::VFSPath>;
	OutputPaths mOutputPaths = {};
};



struct GAMESPRITE_API CharacterPiece
{
	rftl::string mResources = {};
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



struct GAMESPRITE_API CharacterModes
{
	using CategoriesByMode = rftl::unordered_map<rftl::string, CharacterPieceCategories>;
	CategoriesByMode mCategoriesByMode = {};
};

///////////////////////////////////////////////////////////////////////////////

class GAMESPRITE_API CharacterCompositor
{
	RF_NO_COPY( CharacterCompositor );

	//
	// Public methods
public:
	CharacterCompositor( WeakPtr<file::VFS const> vfs, WeakPtr<gfx::ppu::PPUController> ppu );

	bool LoadPieceTables( file::VFSPath const& pieceTablesDir );
	CompositeCharacter CreateCompositeCharacter( CompositeCharacterParams const& params );


	//
	// Private methods
private:
	static sprite::Bitmap CreateCompositeFrame( CompositeFrameParams const& params );
	void WriteFrameToDisk( sprite::Bitmap const& frame, file::VFSPath const& path );
	void CreateCompositeAnims( CompositeAnimParams const& params );
	void CreateCompositeAnim( CompositeAnimParams const& params, CharacterAnimKey const& anim );

	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );
	bool LoadPieceTable( rftl::string mode, CharacterPieceType pieceType, file::VFSPath const& pieceTablePath );
	bool LoadPieceTableDirectory( rftl::string mode, CharacterPieceType pieceType, file::VFSPath const& pieceTableDirectory );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	WeakPtr<gfx::ppu::PPUController> mPpu;
	BitmapCache mBitmapCache;
	CharacterModes mCharacterModes;
};

///////////////////////////////////////////////////////////////////////////////
}
