#include "stdafx.h"
#include "ElementGridSelector.h"

#include "cc3o3/ui/controllers/ElementSlotOverlay.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusEvent.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::ElementGridSelector )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::ElementGridSelector );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

ElementGridSelector::ElementGridSelector( Size size )
	: GenericListBox( character::kMaxSlotsPerElementLevel )
	, mSize( size )
{
	//
}



gfx::ppu::Coord ElementGridSelector::CalcContainerDimensions( Size size )
{
	switch( size )
	{
		case Size::Full:
			return {
				kElementTilesetFull.mTileWidth,
				kElementTilesetFull.mTileHeight *
					static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel ) };
		default:
			RF_DBGFAIL();
			return {};
	}
}



void ElementGridSelector::UpdateFromCharacter( state::ObjectRef const& character )
{
	mCache.UpdateFromCharacter( character, true );
	UpdateDisplay();
}



void ElementGridSelector::UpdateFromCache( ElementGridDisplayCache const& cache )
{
	mCache = cache;
	UpdateDisplay();
}



void ElementGridSelector::DarkenAll()
{
	mCache.DarkenAll();
	UpdateDisplay();
}



character::ElementSlotIndex ElementGridSelector::GetSelectedIndex( UIConstContext const& context ) const
{
	size_t const columnIndex = GetSlotIndexWithSoftFocus( context );
	return character::ElementSlotIndex( mLevel, columnIndex );
}



void ElementGridSelector::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	RF_ASSERT( mTileLayer.NumTiles() > 0 );

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::ppu::Coord const expectedDimensions = CalcContainerDimensions( mSize );
	RF_ASSERT_MSG( container.mAABB.Width() == expectedDimensions.x, "Container not sized as needed" );
	RF_ASSERT_MSG( container.mAABB.Height() == expectedDimensions.y, "Container not sized as needed" );

	mTileLayer.mXCoord = container.mAABB.Left();
	mTileLayer.mYCoord = container.mAABB.Top();
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );

	// Do any of our slots have direct focus?
	InstancedController const* const slotWithFocus = GetSlotWithFocus( context );
	bool const listBoxHasImplicitFocus = slotWithFocus != nullptr;

	// Update colors
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		WeakPtr<ElementSlotOverlay> const slotController = GetMutableSlotController( i );

		if( listBoxHasImplicitFocus )
		{
			if( slotController == slotWithFocus )
			{
				slotController->UpdateState( true, true );
			}
			else
			{
				slotController->UpdateState( true, false );
			}
		}
		else
		{
			slotController->UpdateState( false, false );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void ElementGridSelector::PostInstanceAssign( UIContext& context, Container& container )
{
	gfx::ppu::PPUController const& renderer = GetRenderer( context.GetContainerManager() );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

	ElementTilesetDef tilesetDef = {};
	switch( mSize )
	{
		case Size::Full:
			tilesetDef = kElementTilesetFull;
			break;
		default:
			RF_DBGFAIL();
	}

	RF_ASSERT_ASSUME( tilesetDef.mName != nullptr );
	mTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( tilesetDef.mName );

	RF_ASSERT( tilesetDef.mUsesBorderSlots == false );
	mTileLayer.ClearAndResize( 1, character::kMaxSlotsPerElementLevel );

	RF_ASSERT( tilesetDef.mSupportsText );
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		// TODO: Unify these sizes in a central location
		RF_ASSERT( mSize == Size::Full );

		WeakPtr<ElementSlotOverlay> const overlay =
			AssignSlotController<ElementSlotOverlay>(
				context, i,
				DefaultCreator<ElementSlotOverlay>::Create(
					ElementSlotOverlay::Size::Full ) );
	}
}



bool ElementGridSelector::ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const
{
	// TODO
	return false;
}



bool ElementGridSelector::OnUnhandledFocusEvent( UIContext& context, FocusEvent const& focusEvent )
{
	bool const isPrevious = focusEvent.mEventType == focusevent::Command_NavigateLeft;
	bool const isNext = focusEvent.mEventType == focusevent::Command_NavigateRight;
	bool const isCycle = isPrevious || isNext;

	if( isCycle )
	{
		if( isPrevious && mLevel > 0 )
		{
			mLevel--;
			UpdateDisplay();
			return true;
		}
		else if( isNext && mLevel + 1 < character::kMaxElementLevels )
		{
			mLevel++;
			UpdateDisplay();
			return true;
		}

		// Sink any attempts to move out of selector
		// TODO: Configurable?
		return true;
	}

	return false;
}



WeakPtr<ElementSlotOverlay> ElementGridSelector::GetMutableSlotController( size_t slotIndex )
{
	WeakPtr<ElementSlotOverlay> retVal;
	PtrTransformer<ElementSlotOverlay>::PerformNonTypesafeTransformation(
		GenericListBox::GetMutableSlotController( slotIndex ), retVal );
	return retVal;
}



void ElementGridSelector::UpdateDisplay()
{
	// TODO: Sanitize level if it would be in a column that has no valid slots,
	//  such as when not high enough level to unlock those

	ElementGridDisplayCache::Grid const& grid = mCache.GetGridRef();
	ElementGridDisplayCache::Column const& column = grid.at( mLevel );
	for( size_t i_row = 0; i_row < column.size(); i_row++ )
	{
		ElementGridDisplayCache::Slot const& slot = column.at( i_row );
		mTileLayer.GetMutableTile( 0, i_row ).mIndex = math::enum_bitcast( slot.mTilesetIndex );
		GetMutableSlotController( i_row )->UpdateFromCache( slot );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
