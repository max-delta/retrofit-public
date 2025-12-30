#include "stdafx.h"
#include "AspectColumnSlicer.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/meta/IntegerPromotion.h"


RFTYPE_CREATE_META( RF::ui::controller::AspectColumnSlicer )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::AspectColumnSlicer );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

AspectColumnSlicer::AspectColumnSlicer( Ratio maxWideness, Enableds enabledSlices )
	: mEnabledSlices( enabledSlices )
	, mMaxWideness( maxWideness )
{
	RF_ASSERT( mMaxWideness.IsValid() );
}



ContainerID AspectColumnSlicer::GetChildContainerID( size_t sliceIndex ) const
{
	return mContainers.at( sliceIndex );
}



ContainerID AspectColumnSlicer::CreateChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	return CreateChildContainerInternal( manager, GetMutableContainer( manager, mParentContainerID ), sliceIndex );
}



void AspectColumnSlicer::DestroyChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		DestroyContainer( manager, id );
		id = kInvalidContainerID;
		mEnabledSlices.at( sliceIndex ) = false;
	}
	else
	{
		RF_ASSERT( mEnabledSlices.at( sliceIndex ) == false );
	}
}



void AspectColumnSlicer::OnInstanceAssign( UIContext& context, Container& container )
{
	mParentContainerID = container.mContainerID;

	RF_ASSERT( mAnchors.size() == mEnabledSlices.size() + 1 );
	for( AnchorID& anchor : mAnchors )
	{
		anchor = CreateAnchor( context.GetMutableContainerManager(), container );
	}

	RF_ASSERT( mContainers.size() == mEnabledSlices.size() );
	for( size_t i = 0; i < mContainers.size(); i++ )
	{
		if( mEnabledSlices.at( i ) == false )
		{
			RF_ASSERT( mContainers.at( i ) == kInvalidContainerID );
			mContainers.at( i ) = kInvalidContainerID;
			continue;
		}

		CreateChildContainerInternal( context.GetMutableContainerManager(), container, i );
	}
}



void AspectColumnSlicer::OnAABBRecalc( UIContext& context, Container& container )
{
	using InterfaceType = Ratio::InterfaceType;

	gfx::ppu::AABB const& aabb = container.mAABB;
	gfx::ppu::CoordElem const x0 = aabb.Left();
	gfx::ppu::CoordElem const x100 = aabb.Right();
	gfx::ppu::CoordElem const y0 = aabb.Top();
	gfx::ppu::CoordElem const y100 = aabb.Bottom();
	RF_ASSERT( x0 <= x100 );
	RF_ASSERT( y0 <= y100 );
	InterfaceType const xDelta = math::integer_cast<InterfaceType>( x100 - x0 );
	InterfaceType const yDelta = math::integer_cast<InterfaceType>( y100 - y0 );

	// NOTE: Every anchor except the last one is at y0
	// NOTE: Last anchor is at y100
	MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( 0 ), { x0, y0 } );
	MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( 3 ), { x100, y100 } );

	// What's the parent ratio?
	Ratio const parentRatio = Ratio( xDelta, yDelta );
	RF_ASSERT( parentRatio.IsValid() );

	// Can we just fill it up?
	RF_ASSERT( mMaxWideness.IsValid() );
	if( parentRatio <= mMaxWideness )
	{
		// Still under or equal max wideness, take up entirety of space
		MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( 1 ), { x0, y0 } );
		MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( 2 ), { x100, y0 } );
		return;
	}

	// Above max wideness, need to center and let sides take up the slack

	// (maxWidth / maxHeight) * maxHeight == maxWidth
	InterfaceType const maxWidth = ( mMaxWideness * Ratio( yDelta, 1u ) ).GetAsIntegerViaTruncation();
	RF_ASSERT( maxWidth < xDelta );

	// NOTE: > vs >= because it's theoretically possible to end up in a
	//  situation where we're trying to split a single pixel in half
	InterfaceType const slackToTakeUp = angry_cast<InterfaceType>( xDelta - maxWidth );
	RF_ASSERT( slackToTakeUp > 0 );
	InterfaceType const widthOfEachSide = angry_cast<InterfaceType>( slackToTakeUp / 2 );
	RF_ASSERT( widthOfEachSide >= 0 );

	// Move the anchors
	gfx::ppu::CoordElem const xMiddleLeft = math::integer_cast<gfx::ppu::CoordElem>( x0 + widthOfEachSide );
	gfx::ppu::CoordElem const xMiddleRight = math::integer_cast<gfx::ppu::CoordElem>( x100 - widthOfEachSide );
	RF_ASSERT( xMiddleLeft < xMiddleRight );
	MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( 1 ), { xMiddleLeft, y0 } );
	MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( 2 ), { xMiddleRight, y0 } );
}

///////////////////////////////////////////////////////////////////////////////

ContainerID AspectColumnSlicer::CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		RF_DBGFAIL_MSG( "Container already exists" );
		return kInvalidContainerID;
	}

	AnchorID top = mAnchors.front();
	AnchorID bottom = mAnchors.back();

	AnchorID left = mAnchors.at( sliceIndex );
	AnchorID right = mAnchors.at( sliceIndex + 1 );

	id = Controller::CreateChildContainer( manager, container, left, right, top, bottom );
	return id;
}

///////////////////////////////////////////////////////////////////////////////
}
