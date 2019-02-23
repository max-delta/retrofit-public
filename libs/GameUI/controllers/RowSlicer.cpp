#include "stdafx.h"
#include "RowSlicer.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::ui::controller::RowSlicer )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::RowSlicer );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

RowSlicer::RowSlicer( Ratios const& ratios )
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



ContainerID RowSlicer::GetChildContainerID( size_t sliceIndex ) const
{
	return mContainers.at( sliceIndex );
}



void RowSlicer::CreateChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	CreateChildContainerInternal( manager, GetMutableContainer( manager, mParentContainerID ), sliceIndex );
}



void RowSlicer::DestroyChildContainer( ContainerManager& manager, size_t sliceIndex )
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



void RowSlicer::OnAssign( ContainerManager& manager, Container& container )
{
	mParentContainerID = container.mContainerID;

	RF_ASSERT( mAnchors.size() == mRatios.size() + 1 );
	for( AnchorID& anchor : mAnchors )
	{
		anchor = CreateAnchor( manager, container );
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

		CreateChildContainerInternal( manager, container, i );
	}
}



void RowSlicer::OnAABBRecalc( ContainerManager& manager, Container& container )
{
	Container::AABB4 const& aabb = container.mAABB;
	gfx::PPUCoordElem const x0 = aabb.Left();
	gfx::PPUCoordElem const x100 = aabb.Right();
	gfx::PPUCoordElem const y0 = aabb.Top();
	gfx::PPUCoordElem const y100 = aabb.Bottom();
	gfx::PPUCoordElem const yDelta = y100 - y0;

	// NOTE: Every anchor except the last one is at x0
	// NOTE: Last anchor is at x100
	float rollingY = y0;
	RF_ASSERT( mAnchors.size() == mRatios.size() + 1 );
	for( size_t i = 0; i < mRatios.size(); i++ )
	{
		gfx::PPUCoordElem y = math::integer_truncast<gfx::PPUCoordElem>( rollingY );
		MoveAnchor( manager, mAnchors.at( i ), { x0, y } );
		rollingY += yDelta * mRatios.at( i ).first;
	}
	RF_ASSERT( math::IsWithin<float>( rollingY, 1.f, y100 ) );
	MoveAnchor( manager, mAnchors.back(), { x100, y100 } );
}

///////////////////////////////////////////////////////////////////////////////

void RowSlicer::CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		RF_DBGFAIL_MSG( "Container already exists" );
		return;
	}

	AnchorID top = mAnchors.at( sliceIndex );
	AnchorID bottom = mAnchors.at( sliceIndex + 1 );

	AnchorID left = mAnchors.front();
	AnchorID right = mAnchors.back();

	id = Controller::CreateChildContainer( manager, container, left, right, top, bottom );
}

///////////////////////////////////////////////////////////////////////////////
}}}