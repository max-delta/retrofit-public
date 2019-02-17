#include "stdafx.h"
#include "NineSlicer.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"

#include "core_math/Lerp.h"


namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

NineSlicer::NineSlicer()
{
	for( bool& b : mSliceEnabled )
	{
		b = true;
	}
}



NineSlicer::NineSlicer( bool const ( &sliceEnabled )[9] )
{
	for( size_t i = 0; i < 9; i++ )
	{
		mSliceEnabled[i] = sliceEnabled[i];
	}
}



ContainerID NineSlicer::GetChildContainerID( size_t sliceIndex ) const
{
	return mContainers.at( sliceIndex );
}



void NineSlicer::CreateChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	CreateChildContainerInternal( manager, manager.GetMutableContainer( mParentContainerID ), sliceIndex );
}



void NineSlicer::DestroyChildContainer( ContainerManager& manager, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		manager.DestroyContainer( id );
		id = kInvalidContainerID;
		mSliceEnabled[sliceIndex] = false;
	}
	else
	{
		RF_ASSERT( mSliceEnabled[sliceIndex] == false );
	}
}



void NineSlicer::OnAssign( ContainerManager& manager, Container& container )
{
	mParentContainerID = container.mContainerID;
	m0 = manager.CreateAnchor( container );
	m33 = manager.CreateAnchor( container );
	m66 = manager.CreateAnchor( container );
	m100 = manager.CreateAnchor( container );

	for( size_t i = 0; i < 9; i++ )
	{
		if( mSliceEnabled[i] == false )
		{
			mContainers[i] = kInvalidContainerID;
			continue;
		}

		CreateChildContainerInternal( manager, container, i );
	}
}



void NineSlicer::OnAABBRecalc( ContainerManager& manager, Container& container )
{
	Container::AABB4 const& aabb = container.mAABB;
	gfx::PPUCoordElem const x0 = aabb.Left();
	gfx::PPUCoordElem const x100 = aabb.Right();
	gfx::PPUCoordElem const x33 = math::Lerp( x0, x100, 1.f / 3.f );
	gfx::PPUCoordElem const x66 = math::Lerp( x0, x100, 2.f / 3.f );
	gfx::PPUCoordElem const y0 = aabb.Top();
	gfx::PPUCoordElem const y100 = aabb.Bottom();
	gfx::PPUCoordElem const y33 = math::Lerp( y0, y100, 1.f / 3.f );
	gfx::PPUCoordElem const y66 = math::Lerp( y0, y100, 2.f / 3.f );

	manager.MoveAnchor( m0, { x0, y0 } );
	manager.MoveAnchor( m33, { x33, y33 } );
	manager.MoveAnchor( m66, { x66, y66 } );
	manager.MoveAnchor( m100, { x100, y100 } );
}

///////////////////////////////////////////////////////////////////////////////

void NineSlicer::CreateChildContainerInternal( ContainerManager& manager, Container& container, size_t sliceIndex )
{
	ContainerID& id = mContainers.at( sliceIndex );
	if( id != kInvalidContainerID )
	{
		RF_DBGFAIL_MSG( "Container already exists" );
		return;
	}

	AnchorID top;
	AnchorID bottom;
	if( sliceIndex < 3 )
	{
		top = m0;
		bottom = m33;
	}
	else if( sliceIndex < 6 )
	{
		top = m33;
		bottom = m66;
	}
	else
	{
		top = m66;
		bottom = m100;
	}

	AnchorID left;
	AnchorID right;
	size_t const column = sliceIndex % 3;
	if( column == 0 )
	{
		left = m0;
		right = m33;
	}
	else if( column == 1 )
	{
		left = m33;
		right = m66;
	}
	else
	{
		left = m66;
		right = m100;
	}

	id = manager.CreateChildContainer( container, left, right, top, bottom );
}

///////////////////////////////////////////////////////////////////////////////
}}}
