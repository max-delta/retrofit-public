#include "stdafx.h"
#include "ClampSlicer.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "Logging/Logging.h"


RFTYPE_CREATE_META( RF::ui::controller::ClampSlicer )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::ClampSlicer );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

ClampSlicer::ClampSlicer(
	Params params,
	Mode mode )
	: mMode( mode )
	, mSubtract( params.subtract )
	, mMax( params.max )
	, mStep( params.divisibleBy )
{
	RF_ASSERT( params.subtract >= 0 );
	RF_ASSERT( params.max >= 0 );
	RF_ASSERT( params.divisibleBy >= 0 );

	RF_ASSERT(
		params.subtract > 0 ||
		params.max > 0 ||
		params.divisibleBy > 0 );

	RF_ASSERT( mode != Mode::Invalid );
}



ContainerID ClampSlicer::GetClampedContainerID() const
{
	return mClampedContainer;
}



ContainerID ClampSlicer::GetOverflowContainerID() const
{
	return mOverflowContainer;
}



void ClampSlicer::OnInstanceAssign( UIContext& context, Container& container )
{
	mClampedTopLeftAnchor = CreateAnchor( context.GetMutableContainerManager(), container );
	mClampedBottomRightAnchor = CreateAnchor( context.GetMutableContainerManager(), container );
	mOverflowTopLeftAnchor = CreateAnchor( context.GetMutableContainerManager(), container );
	mOverflowBottomRightAnchor = CreateAnchor( context.GetMutableContainerManager(), container );

	// Clamped
	{
		AnchorID const top = mClampedTopLeftAnchor;
		AnchorID const bottom = mClampedBottomRightAnchor;
		AnchorID const left = mClampedTopLeftAnchor;
		AnchorID const right = mClampedBottomRightAnchor;
		mClampedContainer = Controller::CreateChildContainer( context.GetMutableContainerManager(), container, left, right, top, bottom );
	}

	// Overflow
	{
		AnchorID const top = mOverflowTopLeftAnchor;
		AnchorID const bottom = mOverflowBottomRightAnchor;
		AnchorID const left = mOverflowTopLeftAnchor;
		AnchorID const right = mOverflowBottomRightAnchor;
		mOverflowContainer = Controller::CreateChildContainer( context.GetMutableContainerManager(), container, left, right, top, bottom );
	}
}



void ClampSlicer::OnAABBRecalc( UIContext& context, Container& container )
{
	using gfx::ppu::AABB;
	using gfx::ppu::CoordElem;

	AABB const& bounds = container.mAABB;

	static constexpr auto adjust =
		[]( CoordElem& desired, CoordElem sub, CoordElem max, CoordElem step )
	{
		if( sub > 0 )
		{
			if( desired <= sub )
			{
				RFLOGF_WARNING( nullptr, RFCAT_GAMEUI, "A clamp slicer tried to subtract from its bounds, but that would've made its size zero or negative" );
				RF_DBGFAIL();
			}
			else
			{
				desired -= sub;
			}
		}
		if( max > 0 )
		{
			desired = math::Min( desired, max );
		}
		if( step > 0 )
		{
			desired = math::SnapLowest( desired, step );
		}
	};

	RF_ASSERT( mMode != Mode::Invalid );
	if( mMode == Mode::TotalOverlap || mMode == Mode::Invalid )
	{
		MoveAnchor( context.GetMutableContainerManager(), mClampedTopLeftAnchor, bounds.mTopLeft );
		MoveAnchor( context.GetMutableContainerManager(), mClampedBottomRightAnchor, bounds.mBottomRight );
		MoveAnchor( context.GetMutableContainerManager(), mOverflowTopLeftAnchor, bounds.mTopLeft );
		MoveAnchor( context.GetMutableContainerManager(), mOverflowBottomRightAnchor, bounds.mBottomRight );
		return;
	}

	RF_ASSERT(
		mMode == Mode::ClampLeft_OverflowRight ||
		mMode == Mode::ClampRight_OverflowLeft ||
		mMode == Mode::ClampTop_OverflowBottom ||
		mMode == Mode::ClampBottom_OverflowTop );
	enum class Slice : uint8_t
	{
		Columns = 0,
		Rows,
	};
	Slice const slice =
		( mMode == Mode::ClampLeft_OverflowRight || mMode == Mode::ClampRight_OverflowLeft ) ?
		Slice::Columns :
		Slice::Rows;
	Justification::Value const clampedJustification =
		( mMode == Mode::ClampLeft_OverflowRight || mMode == Mode::ClampTop_OverflowBottom ) ?
		Justification::TopLeft :
		Justification::BottomRight;
	Justification::Value const overflowJustification =
		( mMode == Mode::ClampLeft_OverflowRight || mMode == Mode::ClampTop_OverflowBottom ) ?
		Justification::BottomRight :
		Justification::TopLeft;
	RF_ASSERT( ( clampedJustification & overflowJustification ) == 0 );

	gfx::ppu::Coord clampedDimensions = bounds.Dimensions();
	gfx::ppu::Coord reductionDimensions = { 0, 0 };
	if( slice == Slice::Columns )
	{
		adjust( clampedDimensions.x, mSubtract, mMax, mStep );
		reductionDimensions.x = clampedDimensions.x;
	}
	else
	{
		RF_ASSERT( slice == Slice::Rows );
		adjust( clampedDimensions.y, mSubtract, mMax, mStep );
		reductionDimensions.y = clampedDimensions.y;
	}
	RF_ASSERT( clampedDimensions.x <= bounds.Width() );
	RF_ASSERT( clampedDimensions.y <= bounds.Height() );
	gfx::ppu::Coord const overflowDimensions = bounds.Dimensions() - reductionDimensions;
	RF_ASSERT( overflowDimensions.x > 0 );
	RF_ASSERT( overflowDimensions.y > 0 );

	gfx::ppu::Coord const clampedPos = AlignToJustify( clampedDimensions, container.mAABB, clampedJustification );
	gfx::ppu::Coord const overflowPos = AlignToJustify( overflowDimensions, container.mAABB, overflowJustification );

	MoveAnchor( context.GetMutableContainerManager(), mClampedTopLeftAnchor, clampedPos );
	MoveAnchor( context.GetMutableContainerManager(), mClampedBottomRightAnchor, clampedPos + clampedDimensions );
	MoveAnchor( context.GetMutableContainerManager(), mOverflowTopLeftAnchor, overflowPos );
	MoveAnchor( context.GetMutableContainerManager(), mOverflowBottomRightAnchor, overflowPos + overflowDimensions );
}

///////////////////////////////////////////////////////////////////////////////
}
