#include "stdafx.h"
#include "BorderFrame.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/Lerp.h"


RFTYPE_CREATE_META( RF::ui::controller::BorderFrame )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::BorderFrame );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

void BorderFrame::SetTileset( ui::UIContext& context, gfx::ManagedTilesetID tileset, gfx::PPUCoord expectedTileDimensions, gfx::PPUCoord paddingDimensions )
{
	mTileLayer.mTilesetReference = tileset;
	mExpectedTileDimensions = expectedTileDimensions;
	mPaddingDimensions = paddingDimensions;

	// Invalidate tilemap and wait for recalc
	mTileLayer.Clear();
	context.GetMutableContainerManager().RequestHardRecalc( GetContainerID() );
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

	gfx::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::PPUCoord pos = container.mAABB.mTopLeft;
	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Top ) )
	{
		pos.y = container.mAABB.Top();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Bottom ) )
	{
		pos.y = container.mAABB.Bottom() - mExpectedDimensions.y;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Middle ) )
	{
		gfx::PPUCoordElem const top = container.mAABB.Top();
		gfx::PPUCoordElem const bottom = container.mAABB.Bottom() - mExpectedDimensions.y;
		pos.y = math::Lerp( top, bottom, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

	if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Left ) )
	{
		pos.x = container.mAABB.Left();
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Right ) )
	{
		pos.x = container.mAABB.Right() - mExpectedDimensions.x;
	}
	else if( math::enum_bitcast( mJustification ) & math::enum_bitcast( Justification::Center ) )
	{
		gfx::PPUCoordElem const left = container.mAABB.Left();
		gfx::PPUCoordElem const right = container.mAABB.Right() - mExpectedDimensions.x;
		pos.x = math::Lerp( left, right, 0.5f );
	}
	else
	{
		RF_DBGFAIL();
	}

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

	gfx::PPUCoord const topLeft = container.mAABB.mTopLeft + ( mExpectedTileDimensions + mPaddingDimensions );
	gfx::PPUCoord const bottomRight = container.mAABB.mBottomRight - ( mExpectedTileDimensions + mPaddingDimensions );
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

	gfx::PPUCoordElem const availableWidth = container.mAABB.Width();
	gfx::PPUCoordElem const availableHeight = container.mAABB.Height();
	size_t const numAvailableColumns = math::integer_cast<size_t>( availableWidth / mExpectedTileDimensions.x );
	size_t const numAvailableRows = math::integer_cast<size_t>( availableHeight / mExpectedTileDimensions.y );

	// Needs to be atleast 2x2, even if the AABB isn't big enough to fit it
	size_t const numColumns = math::Max<size_t>( numAvailableColumns, 2 );
	size_t const numRows = math::Max<size_t>( numAvailableRows, 2 );

	mExpectedDimensions.x = mExpectedTileDimensions.x * math::integer_cast<gfx::PPUCoordElem>( numColumns );
	mExpectedDimensions.y = mExpectedTileDimensions.y * math::integer_cast<gfx::PPUCoordElem>( numRows );

	mTileLayer.ClearAndResize( numColumns, numRows );

	// Frame tilesets are laid out as follows:
	// 0 1 2
	// 3 4 5
	// 6 7 8
	//
	//  /-\
	//  |#|
	//  \-/

	// Fill in center
	for( size_t x = 1; x < numColumns - 1; x++ )
	{
		for( size_t y = 1; y < numRows - 1; y++ )
		{
			mTileLayer.GetMutableTile( x, y ).mIndex = 4;
		}
	}

	// Fill in sides
	for( size_t x = 1; x < numColumns - 1; x++ )
	{
		mTileLayer.GetMutableTile( x, 0 ).mIndex = 1;
		mTileLayer.GetMutableTile( x, numRows - 1 ).mIndex = 7;
	}
	for( size_t y = 1; y < numRows - 1; y++ )
	{
		mTileLayer.GetMutableTile( 0, y ).mIndex = 3;
		mTileLayer.GetMutableTile( numColumns - 1, y ).mIndex = 5;
	}

	// Fill in corners
	// NOTE: In 2x2 case, this is the only thing that will actually get run
	mTileLayer.GetMutableTile( 0, 0 ).mIndex = 0;
	mTileLayer.GetMutableTile( 0, numRows - 1 ).mIndex = 6;
	mTileLayer.GetMutableTile( numColumns - 1, 0 ).mIndex = 2;
	mTileLayer.GetMutableTile( numColumns - 1, numRows - 1 ).mIndex = 8;
}

///////////////////////////////////////////////////////////////////////////////
}}}
