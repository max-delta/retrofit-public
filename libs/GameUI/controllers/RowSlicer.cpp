#include "stdafx.h"
#include "RowSlicer.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::RowSlicer )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::RowSlicer );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

RowSlicer::RowSlicer( Ratios const& ratios )
	: mAnchors( ratios.size() + 1, kInvalidAnchorID )
	, mContainers( ratios.size(), kInvalidContainerID )
	, mRatios( ratios )
{
	RF_ASSERT( mRatios.size() >= 2 );
	if constexpr( config::kAsserts )
	{
		float total = 0.f;
		for( Ratios::value_type const& ratio : mRatios )
		{
			total += ratio.first;
		}
		RF_ASSERT( math::Equals( total, 1.f ) );
	}
}



RowSlicer::RowSlicer( size_t numSlices )
	: mAnchors( numSlices + 1, kInvalidAnchorID )
	, mContainers( numSlices, kInvalidContainerID )
{
	RF_ASSERT( numSlices >= 2 );
	float const ratio = 1.f / math::float_cast<float>( numSlices );
	for( size_t i = 0; i < numSlices; i++ )
	{
		mRatios.emplace_back( ratio, true );
	}
}



ContainerID RowSlicer::GetChildContainerID( size_t sliceIndex ) const
{
	return mContainers.at( sliceIndex );
}



ContainerID RowSlicer::CreateChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	return CreateChildContainerInternal( manager, GetMutableContainer( manager, mParentContainerID ), sliceIndex );
}



void RowSlicer::DestroyChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		DestroyContainer( manager, id );
		id = kInvalidContainerID;
		mRatios.at( sliceIndex ).second = false;
	}
	else
	{
		RF_ASSERT( mRatios.at( sliceIndex ).second == false );
	}
}



void RowSlicer::OnInstanceAssign( UIContext& context, Container& container )
{
	mParentContainerID = container.mContainerID;

	RF_ASSERT( mAnchors.size() == mRatios.size() + 1 );
	for( AnchorID& anchor : mAnchors )
	{
		anchor = CreateAnchor( context.GetMutableContainerManager(), container );
	}

	RF_ASSERT( mContainers.size() == mRatios.size() );
	for( size_t i = 0; i < mContainers.size(); i++ )
	{
		if( mRatios.at( i ).second == false )
		{
			RF_ASSERT( mContainers.at( i ) == kInvalidContainerID );
			mContainers.at( i ) = kInvalidContainerID;
			continue;
		}

		CreateChildContainerInternal( context.GetMutableContainerManager(), container, i );
	}
}



void RowSlicer::OnAABBRecalc( UIContext& context, Container& container )
{
	gfx::ppu::AABB const& aabb = container.mAABB;
	gfx::ppu::CoordElem const x0 = aabb.Left();
	gfx::ppu::CoordElem const x100 = aabb.Right();
	gfx::ppu::CoordElem const y0 = aabb.Top();
	gfx::ppu::CoordElem const y100 = aabb.Bottom();
	gfx::ppu::CoordElem const yDelta = y100 - y0;

	// NOTE: Every anchor except the last one is at x0
	// NOTE: Last anchor is at x100
	float rollingY = y0;
	RF_ASSERT( mAnchors.size() == mRatios.size() + 1 );
	for( size_t i = 0; i < mRatios.size(); i++ )
	{
		gfx::ppu::CoordElem y = math::integer_truncast<gfx::ppu::CoordElem>( rollingY );
		MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( i ), { x0, y } );
		rollingY += math::float_cast<float>( yDelta ) * mRatios.at( i ).first;
	}
	RF_ASSERT( math::IsWithin<float>( rollingY, 1.f, y100 ) );
	MoveAnchor( context.GetMutableContainerManager(), mAnchors.back(), { x100, y100 } );
}

///////////////////////////////////////////////////////////////////////////////

ContainerID RowSlicer::CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		RF_DBGFAIL_MSG( "Container already exists" );
		return kInvalidContainerID;
	}

	AnchorID top = mAnchors.at( sliceIndex );
	AnchorID bottom = mAnchors.at( sliceIndex + 1 );

	AnchorID left = mAnchors.front();
	AnchorID right = mAnchors.back();

	id = Controller::CreateChildContainer( manager, container, left, right, top, bottom );
	return id;
}

///////////////////////////////////////////////////////////////////////////////
}
