#include "stdafx.h"
#include "BorderFrame.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::BorderFrame )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::BorderFrame );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

void BorderFrame::SetTileset(
	ui::UIContext& context,
	gfx::ManagedTilesetID tileset,
	gfx::ppu::Coord expectedTileDimensions,
	gfx::ppu::Coord expectedPatternDimensions,
	gfx::ppu::Coord paddingDimensions )
{
	mTileLayer.mTilesetReference = tileset;
	mExpectedTileDimensions = expectedTileDimensions;
	mExpectedPatternDimensions = expectedPatternDimensions;
	mPaddingDimensions = paddingDimensions;

	// Invalidate tilemap and wait for recalc
	mTileLayer.Clear();
	RequestHardRecalc( context );
}



void BorderFrame::SetJustification( Justification justification )
{
	mJustification = justification;
}



ContainerID BorderFrame::GetChildContainerID() const
{
	return mChildContainerID;
}



void BorderFrame::OnInstanceAssign( UIContext& context, Container& container )
{
	mTopLeftAnchor = CreateAnchor( context.GetMutableContainerManager(), container );
	mBottomRightAnchor = CreateAnchor( context.GetMutableContainerManager(), container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mChildContainerID = Controller::CreateChildContainer( context.GetMutableContainerManager(), container, left, right, top, bottom );
}



void BorderFrame::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	RF_ASSERT( mTileLayer.NumTiles() > 0 );

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::ppu::Coord const pos = AlignToJustify( mExpectedDimensions, container.mAABB, mJustification );

	mTileLayer.mXCoord = pos.x;
	mTileLayer.mYCoord = pos.y;
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );
}



void BorderFrame::OnAABBRecalc( UIContext& context, Container& container )
{
	RecalcTilemap( container );

	gfx::ppu::Coord const topLeft = container.mAABB.mTopLeft + ( mExpectedTileDimensions + mPaddingDimensions );
	gfx::ppu::Coord const bottomRight = container.mAABB.mBottomRight - ( mExpectedTileDimensions + mPaddingDimensions );
	MoveAnchor( context.GetMutableContainerManager(), mTopLeftAnchor, topLeft );
	MoveAnchor( context.GetMutableContainerManager(), mBottomRightAnchor, bottomRight );
}

///////////////////////////////////////////////////////////////////////////////

void BorderFrame::RecalcTilemap( Container const& container )
{
	if(
		mExpectedTileDimensions.x == 0 ||
		mExpectedTileDimensions.y == 0 )
	{
		// No tileset
		mTileLayer.Clear();
		return;
	}

	gfx::ppu::CoordElem const availableWidth = container.mAABB.Width();
	gfx::ppu::CoordElem const availableHeight = container.mAABB.Height();
	size_t const numAvailableColumns = math::integer_cast<size_t>( availableWidth / mExpectedTileDimensions.x );
	size_t const numAvailableRows = math::integer_cast<size_t>( availableHeight / mExpectedTileDimensions.y );

	gfx::ppu::TileLayer::TileIndex const numInputColumns = mExpectedPatternDimensions.x / mExpectedTileDimensions.x;
	gfx::ppu::TileLayer::TileIndex const numInputRows = mExpectedPatternDimensions.y / mExpectedTileDimensions.y;

	// Needs to be atleast 2x2, even if the AABB isn't big enough to fit it
	size_t const numOutputColumns = math::Max<size_t>( numAvailableColumns, 2 );
	size_t const numOutputRows = math::Max<size_t>( numAvailableRows, 2 );

	mExpectedDimensions.x = mExpectedTileDimensions.x * math::integer_cast<gfx::ppu::CoordElem>( numOutputColumns );
	mExpectedDimensions.y = mExpectedTileDimensions.y * math::integer_cast<gfx::ppu::CoordElem>( numOutputRows );

	mTileLayer.ClearAndResize( numOutputColumns, numOutputRows );

	// Frame tilesets are laid out in varying sizes with this pattern:
	// / - \
	// | # |
	// \ - /
	// The interior space '#' and edges '|' or '-' may be multiple tiles
	const gfx::ppu::TileLayer::TileIndex patternCountX = numInputColumns - 2;
	const gfx::ppu::TileLayer::TileIndex patternCountY = numInputRows - 2;
	gfx::ppu::TileLayer::TileIndex patternX;
	gfx::ppu::TileLayer::TileIndex patternY;

	// Fill in corners
	// NOTE: In 2x2 case, this is the only thing that will actually get run
	const gfx::ppu::TileLayer::TileIndex corner0 = 0;
	const gfx::ppu::TileLayer::TileIndex corner1 = numInputColumns - 1;
	const gfx::ppu::TileLayer::TileIndex corner2 = numInputColumns * ( numInputRows - 1 );
	const gfx::ppu::TileLayer::TileIndex corner3 = ( numInputColumns * numInputRows ) - 1;
	mTileLayer.GetMutableTile( 0, 0 ).mIndex = corner0;
	mTileLayer.GetMutableTile( numOutputColumns - 1, 0 ).mIndex = corner1;
	mTileLayer.GetMutableTile( 0, numOutputRows - 1 ).mIndex = corner2;
	mTileLayer.GetMutableTile( numOutputColumns - 1, numOutputRows - 1 ).mIndex = corner3;

	// Fill in horizontal sides
	const gfx::ppu::TileLayer::TileIndex topStart = corner0 + 1;
	const gfx::ppu::TileLayer::TileIndex bottomStart = corner2 + 1;
	patternX = 0;
	for( size_t x = 1; x < numOutputColumns - 1; x++ )
	{
		if( patternX >= patternCountX )
		{
			patternX = 0;
		}

		mTileLayer.GetMutableTile( x, 0 ).mIndex = topStart + patternX;
		mTileLayer.GetMutableTile( x, numOutputRows - 1 ).mIndex = bottomStart + patternX;

		patternX++;
	}

	// Fill in vertical sides
	const gfx::ppu::TileLayer::TileIndex leftStart = corner0 + numInputColumns;
	const gfx::ppu::TileLayer::TileIndex rightStart = corner1 + numInputColumns;
	patternY = 0;
	for( size_t y = 1; y < numOutputRows - 1; y++ )
	{
		if( patternY >= patternCountY )
		{
			patternY = 0;
		}

		mTileLayer.GetMutableTile( 0, y ).mIndex = leftStart + patternY * numInputColumns;
		mTileLayer.GetMutableTile( numOutputColumns - 1, y ).mIndex = rightStart + patternY * numInputColumns;

		patternY++;
	}

	// Fill in center
	const gfx::ppu::TileLayer::TileIndex centerStart = leftStart + 1;
	patternX = 0;
	for( size_t x = 1; x < numOutputColumns - 1; x++ )
	{
		if( patternX >= patternCountX )
		{
			patternX = 0;
		}

		patternY = 0;
		for( size_t y = 1; y < numOutputRows - 1; y++ )
		{
			if( patternY >= patternCountY )
			{
				patternY = 0;
			}

			mTileLayer.GetMutableTile( x, y ).mIndex = centerStart + patternX + patternY * numInputColumns;

			patternY++;
		}

		patternX++;
	}
}

///////////////////////////////////////////////////////////////////////////////
}
