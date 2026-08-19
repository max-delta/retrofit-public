#include "stdafx.h"
#include "TileLayerDisplay.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/Lerp.h"


RFTYPE_CREATE_META( RF::ui::controller::TileLayerDisplay )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::TileLayerDisplay );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

void TileLayerDisplay::SetTileset( gfx::ManagedTilesetID tileset, gfx::ppu::CoordElem expectedTileWidth, gfx::ppu::CoordElem expectedTileHeight )
{
	mTileLayer.mTilesetReference = tileset;
	mExpectedTileDimensions = { expectedTileWidth, expectedTileHeight };
}



void TileLayerDisplay::SetJustification( Justification::Value justification )
{
	mJustification = justification;
}



gfx::ppu::TileLayer& TileLayerDisplay::GetMutableTileLayer()
{
	return mTileLayer;
}



void TileLayerDisplay::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::ppu::Coord const expectedDimensions = {
		math::integer_cast<gfx::ppu::CoordElem>(
			mExpectedTileDimensions.x *
			math::integer_cast<gfx::ppu::CoordElem>(
				mTileLayer.NumColumns() ) ),
		math::integer_cast<gfx::ppu::CoordElem>(
			mExpectedTileDimensions.y *
			math::integer_cast<gfx::ppu::CoordElem>(
				mTileLayer.NumRows() ) ) };
	gfx::ppu::AABB const& bounds = container.mAABB;
	gfx::ppu::Coord const pos = AlignToJustify( expectedDimensions, bounds, mJustification );

	mTileLayer.mXCoord = pos.x;
	mTileLayer.mYCoord = pos.y;
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );
}

///////////////////////////////////////////////////////////////////////////////
}
