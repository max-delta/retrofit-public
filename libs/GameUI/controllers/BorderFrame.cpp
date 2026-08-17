#include "stdafx.h"
#include "BorderFrame.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/meta/IntegerPromotion.h"


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
	BorderFrameShape const& shape )
{
	mTileLayer.mTilesetReference = tileset;
	mShape = shape;

	// Invalidate tilemap and wait for recalc
	mTileLayer.Clear();
	RequestHardRecalc( context );
}



void BorderFrame::SetJustification( Justification::Value justification )
{
	mJustification = justification;
}



void BorderFrame::SwapPeerTileset( gfx::ManagedTilesetID tileset )
{
	mTileLayer.mTilesetReference = tileset;
}



ContainerID BorderFrame::GetChildContainerID() const
{
	return mChildContainerID;
}



void BorderFrame::OnInstanceAssign( UIContext& context, Container& container )
{
	ContainerManager& uiManager = context.GetMutableContainerManager();

	mTopLeftAnchor = CreateAnchor( uiManager, container );
	mBottomRightAnchor = CreateAnchor( uiManager, container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mChildContainerID = Controller::CreateChildContainer( uiManager, container, left, right, top, bottom );
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

	gfx::ppu::Coord const topLeft = container.mAABB.mTopLeft + ( mShape.mExpectedTileDimensions + mShape.mPaddingDimensions );
	gfx::ppu::Coord const bottomRight = container.mAABB.mBottomRight - ( mShape.mExpectedTileDimensions + mShape.mPaddingDimensions );
	MoveAnchor( context.GetMutableContainerManager(), mTopLeftAnchor, topLeft );
	MoveAnchor( context.GetMutableContainerManager(), mBottomRightAnchor, bottomRight );
}

///////////////////////////////////////////////////////////////////////////////

void BorderFrame::RecalcTilemap( Container const& container )
{
	if(
		mShape.mExpectedTileDimensions.x == 0 ||
		mShape.mExpectedTileDimensions.y == 0 )
	{
		// No tileset
		mTileLayer.Clear();
		return;
	}

	gfx::ppu::CoordElem const availableWidth = container.mAABB.Width();
	gfx::ppu::CoordElem const availableHeight = container.mAABB.Height();
	size_t const numAvailableColumns = math::integer_cast<size_t>( availableWidth / mShape.mExpectedTileDimensions.x );
	size_t const numAvailableRows = math::integer_cast<size_t>( availableHeight / mShape.mExpectedTileDimensions.y );

	gfx::ppu::TileLayer::TileIndex const numInputColumns =
		math::integer_cast<gfx::ppu::TileLayer::TileIndex>(
			mShape.mExpectedPatternDimensions.x / mShape.mExpectedTileDimensions.x );
	gfx::ppu::TileLayer::TileIndex const numInputRows =
		math::integer_cast<gfx::ppu::TileLayer::TileIndex>(
			mShape.mExpectedPatternDimensions.y / mShape.mExpectedTileDimensions.y );

	// Needs to be atleast 2x2, even if the AABB isn't big enough to fit it
	size_t const numOutputColumns = math::Max<size_t>( numAvailableColumns, 2 );
	size_t const numOutputRows = math::Max<size_t>( numAvailableRows, 2 );

	mExpectedDimensions.x = mShape.mExpectedTileDimensions.x * math::integer_cast<gfx::ppu::CoordElem>( numOutputColumns );
	mExpectedDimensions.y = mShape.mExpectedTileDimensions.y * math::integer_cast<gfx::ppu::CoordElem>( numOutputRows );

	mTileLayer.ClearAndResize( numOutputColumns, numOutputRows );

	// Frame tilesets are laid out in varying sizes with this pattern:
	// / - \
	// | # |
	// \ - /
	// The interior space '#' and edges '|' or '-' may be multiple tiles
	RF_ASSERT( numInputColumns >= 2 );
	RF_ASSERT( numInputRows >= 2 );
	const gfx::ppu::TileLayer::TileIndex patternCountX = numInputColumns - 2u;
	const gfx::ppu::TileLayer::TileIndex patternCountY = numInputRows - 2u;
	gfx::ppu::TileLayer::TileIndex patternX;
	gfx::ppu::TileLayer::TileIndex patternY;

	// Fill in corners
	// NOTE: In 2x2 case, this is the only thing that will actually get run
	const gfx::ppu::TileLayer::TileIndex corner0 = 0;
	const gfx::ppu::TileLayer::TileIndex corner1 = numInputColumns - 1u;
	const gfx::ppu::TileLayer::TileIndex corner2 = numInputColumns * ( numInputRows - 1u );
	const gfx::ppu::TileLayer::TileIndex corner3 = ( numInputColumns * numInputRows ) - 1u;
	mTileLayer.GetMutableTile( 0, 0 ).SetIndex( corner0 );
	mTileLayer.GetMutableTile( numOutputColumns - 1, 0 ).SetIndex( corner1 );
	mTileLayer.GetMutableTile( 0, numOutputRows - 1 ).SetIndex( corner2 );
	mTileLayer.GetMutableTile( numOutputColumns - 1, numOutputRows - 1 ).SetIndex( corner3 );

	// Fill in horizontal sides
	const gfx::ppu::TileLayer::TileIndex topStart = corner0 + 1u;
	const gfx::ppu::TileLayer::TileIndex bottomStart = corner2 + 1u;
	patternX = 0;
	for( size_t x = 1; x < numOutputColumns - 1; x++ )
	{
		if( patternX >= patternCountX )
		{
			patternX = 0;
		}

		mTileLayer.GetMutableTile( x, 0 ).SetIndex( // Clang-format is trash garbage
			angry_cast<gfx::ppu::TileLayer::TileIndex>( // Clang-format is trash garbage
				topStart + patternX ) );
		mTileLayer.GetMutableTile( x, numOutputRows - 1 ).SetIndex( // Clang-format is trash garbage
			angry_cast<gfx::ppu::TileLayer::TileIndex>( // Clang-format is trash garbage
				bottomStart + patternX ) );

		patternX++;
	}

	// Fill in vertical sides
	const gfx::ppu::TileLayer::TileIndex leftStart =
		angry_cast<gfx::ppu::TileLayer::TileIndex>(
			corner0 + numInputColumns );
	const gfx::ppu::TileLayer::TileIndex rightStart =
		angry_cast<gfx::ppu::TileLayer::TileIndex>(
			corner1 + numInputColumns );
	patternY = 0;
	for( size_t y = 1; y < numOutputRows - 1; y++ )
	{
		if( patternY >= patternCountY )
		{
			patternY = 0;
		}

		mTileLayer.GetMutableTile( 0, y ).SetIndex( // Clang-format is trash garbage
			angry_cast<gfx::ppu::TileLayer::TileIndex>( // Clang-format is trash garbage
				leftStart + patternY * numInputColumns ) );
		mTileLayer.GetMutableTile( numOutputColumns - 1, y ).SetIndex( // Clang-format is trash garbage
			angry_cast<gfx::ppu::TileLayer::TileIndex>( // Clang-format is trash garbage
				rightStart + patternY * numInputColumns ) );

		patternY++;
	}

	// Fill in center
	const gfx::ppu::TileLayer::TileIndex centerStart = leftStart + 1u;
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

			mTileLayer.GetMutableTile( x, y ).SetIndex(
				angry_cast<gfx::ppu::TileLayer::TileIndex>(
					centerStart + patternX + patternY * numInputColumns ) );

			patternY++;
		}

		patternX++;
	}
}

///////////////////////////////////////////////////////////////////////////////
}
