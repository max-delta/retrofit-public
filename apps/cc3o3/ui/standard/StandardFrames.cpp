#include "stdafx.h"
#include "StandardFrames.h"

#include "PPU/Tileset.h"
#include "PPU/TilesetManager.h"
#include "PPU/PPUController.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

BorderFrameDef QueryBorderFrameDef(
	gfx::ppu::PPUController const& ppu,
	BorderFrameProto const& proto )
{
	return QueryBorderFrameDef(
		*ppu.GetTilesetManager(),
		proto );
}

BorderFrameDef QueryBorderFrameDef(
	gfx::TilesetManager const& tsetMan,
	BorderFrameProto const& proto )
{
	gfx::ManagedTilesetID const tsetID = tsetMan.GetManagedResourceIDFromResourceName( proto.mResourceName );
	RF_ASSERT( tsetID != gfx::kInvalidManagedTilesetID );
	WeakPtr<gfx::Tileset const> const tsetPtr = tsetMan.GetResourceFromManagedResourceID( tsetID );
	RFLOG_TEST_AND_FATAL( tsetPtr != nullptr, proto.mResourceName, RFCAT_CC3O3, "Expected a tileset to already be loaded, and it is not" );
	gfx::Tileset const& tset = *tsetPtr;

	gfx::ppu::Coord const& expectedSize = proto.mBorderShape.mExpectedTileDimensions;
	gfx::ppu::Coord const actualSize{
		math::integer_cast<gfx::ppu::CoordElem>( tset.DebugGetWidth() ),
		math::integer_cast<gfx::ppu::CoordElem>( tset.DebugGetHeight() ) };
	RFLOG_TEST_AND_NOTIFY(
		actualSize == expectedSize,
		proto.mResourceName,
		RFCAT_CC3O3,
		"Expected a border frame tileset to be {}x{}px, but it's {}x{}px",
		expectedSize.x, expectedSize.y,
		actualSize.x, actualSize.y );

	BorderFrameDef const retVal = {
		.mResourceName = proto.mResourceName,
		.mManagedID = tsetID,
		.mBorderShape = proto.mBorderShape,
	};

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
