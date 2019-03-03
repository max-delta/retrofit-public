#include "stdafx.h"
#include "ColumnSlicer.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::ColumnSlicer )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::ColumnSlicer );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

ColumnSlicer::ColumnSlicer( Ratios const& ratios )
	: mAnchors( ratios.size() + 1, kInvalidAnchorID )
	, mContainers( ratios.size(), kInvalidContainerID )
	, mRatios( ratios )
{
	RF_ASSERT( mRatios.size() >= 2 );
	#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	{
		float total = 0.f;
		for( Ratios::value_type const& ratio : mRatios )
		{
			total += ratio.first;
		}
		RF_ASSERT( math::Equals( total, 1.f ) );
	}
	#endif
}



ColumnSlicer::ColumnSlicer( size_t numSlices )
	: mAnchors( numSlices + 1, kInvalidAnchorID )
	, mContainers( numSlices, kInvalidContainerID )
{
	RF_ASSERT( numSlices >= 2 );
	float const ratio = 1.f / numSlices;
	for( size_t i = 0; i < numSlices; i++ )
	{
		mRatios.emplace_back( ratio, true );
	}
}



ContainerID ColumnSlicer::GetChildContainerID( size_t sliceIndex ) const
{
	return mContainers.at( sliceIndex );
}



void ColumnSlicer::CreateChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	CreateChildContainerInternal( manager, GetMutableContainer( manager, mParentContainerID ), sliceIndex );
}



void ColumnSlicer::DestroyChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		DestroyContainer( manager, id );
		id = kInvalidContainerID;
		mRatios.at(sliceIndex).second = false;
	}
	else
	{
		RF_ASSERT( mRatios.at( sliceIndex ).second == false );
	}
}



void ColumnSlicer::OnAssign( UIContext& context, Container& container )
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



void ColumnSlicer::OnAABBRecalc( UIContext& context, Container& container )
{
	Container::AABB4 const& aabb = container.mAABB;
	gfx::PPUCoordElem const x0 = aabb.Left();
	gfx::PPUCoordElem const x100 = aabb.Right();
	gfx::PPUCoordElem const xDelta = x100 - x0;
	gfx::PPUCoordElem const y0 = aabb.Top();
	gfx::PPUCoordElem const y100 = aabb.Bottom();

	// NOTE: Every anchor except the last one is at y0
	// NOTE: Last anchor is at y100
	float rollingX = x0;
	RF_ASSERT( mAnchors.size() == mRatios.size() + 1 );
	for( size_t i = 0; i < mRatios.size(); i++ )
	{
		gfx::PPUCoordElem x = math::integer_truncast<gfx::PPUCoordElem>( rollingX );
		MoveAnchor( context.GetMutableContainerManager(), mAnchors.at( i ), { x, y0 } );
		rollingX += xDelta * mRatios.at( i ).first;
	}
	RF_ASSERT( math::IsWithin<float>( rollingX, 1.f, x100 ) );
	MoveAnchor( context.GetMutableContainerManager(), mAnchors.back(), { x100, y100 } );
}

///////////////////////////////////////////////////////////////////////////////

void ColumnSlicer::CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		RF_DBGFAIL_MSG( "Container already exists" );
		return;
	}

	AnchorID top = mAnchors.front();
	AnchorID bottom = mAnchors.back();

	AnchorID left = mAnchors.at( sliceIndex );
	AnchorID right = mAnchors.at( sliceIndex + 1 );

	id = Controller::CreateChildContainer( manager, container, left, right, top, bottom );
}

///////////////////////////////////////////////////////////////////////////////
}}}
