#include "stdafx.h"
#include "Clamper.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/AlignmentHelpers.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "Logging/Logging.h"


RFTYPE_CREATE_META( RF::ui::controller::Clamper )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::Clamper );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

Clamper::Clamper(
	Params params,
	Justification::Value justification )
	: mJustification( justification )
	, mSubtractDimensions( params.subtractWidth, params.subtractHeight )
	, mMaxDimensions( params.maxWidth, params.maxHeight )
	, mStepDimensions( params.divisibleByWidth, params.divisibleByHeight )
{
	RF_ASSERT( params.subtractWidth >= 0 );
	RF_ASSERT( params.subtractHeight >= 0 );
	RF_ASSERT( params.maxWidth >= 0 );
	RF_ASSERT( params.maxHeight >= 0 );
	RF_ASSERT( params.divisibleByWidth >= 0 );
	RF_ASSERT( params.divisibleByHeight >= 0 );

	RF_ASSERT(
		params.subtractWidth > 0 ||
		params.subtractHeight > 0 ||
		params.maxWidth > 0 ||
		params.maxHeight > 0 ||
		params.divisibleByWidth > 0 ||
		params.divisibleByHeight > 0 );

	RF_ASSERT( justification != Justification::Invalid );
}



ContainerID Clamper::GetChildContainerID() const
{
	return mContainer;
}



void Clamper::OnInstanceAssign( UIContext& context, Container& container )
{
	mTopLeftAnchor = CreateAnchor( context.GetMutableContainerManager(), container );
	mBottomRightAnchor = CreateAnchor( context.GetMutableContainerManager(), container );

	AnchorID const top = mTopLeftAnchor;
	AnchorID const bottom = mBottomRightAnchor;
	AnchorID const left = mTopLeftAnchor;
	AnchorID const right = mBottomRightAnchor;
	mContainer = Controller::CreateChildContainer( context.GetMutableContainerManager(), container, left, right, top, bottom );
}



void Clamper::OnAABBRecalc( UIContext& context, Container& container )
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
				RFLOGF_WARNING( nullptr, RFCAT_GAMEUI, "A clamper tried to subtract from its bounds, but that would've made its size zero or negative" );
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

	gfx::ppu::Coord desiredDimensions = bounds.Dimensions();
	adjust( desiredDimensions.x, mSubtractDimensions.x, mMaxDimensions.x, mStepDimensions.x );
	adjust( desiredDimensions.y, mSubtractDimensions.y, mMaxDimensions.y, mStepDimensions.y );
	RF_ASSERT( desiredDimensions.x <= bounds.Width() );
	RF_ASSERT( desiredDimensions.y <= bounds.Height() );

	gfx::ppu::Coord const pos = AlignToJustify( desiredDimensions, container.mAABB, mJustification );

	MoveAnchor( context.GetMutableContainerManager(), mTopLeftAnchor, pos );
	MoveAnchor( context.GetMutableContainerManager(), mBottomRightAnchor, pos + desiredDimensions );
}

///////////////////////////////////////////////////////////////////////////////
}
