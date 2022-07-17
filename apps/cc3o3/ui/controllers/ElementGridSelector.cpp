#include "stdafx.h"
#include "ElementGridSelector.h"

#include "cc3o3/ui/controllers/ElementSlotOverlay.h"

#include "GameUI/AlignmentHelpers.h"
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

ElementGridSelector::ElementGridSelector( ElementTileSize size )
	: GenericListBox( character::kMaxSlotsPerElementLevel )
	, mSize( size )
{
	//
}



gfx::ppu::Coord ElementGridSelector::CalcContainerDimensions( ElementTileSize size )
{
	switch( size )
	{
		case ElementTileSize::Full:
			return {
				kElementTilesetFull.mTileWidth,
				kElementTilesetFull.mTileHeight *
					static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel ) };
		case ElementTileSize::Medium:
			return {
				kElementTilesetMedium.mTileWidth,
				kElementTilesetMedium.mTileHeight *
					static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel ) };
		case ElementTileSize::Micro:
		case ElementTileSize::Mini:
		case ElementTileSize::Invalid:
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
	RF_ASSERT( mMainTileLayer.NumTiles() > 0 );
	RF_ASSERT( mLeftWingTileLayer.NumTiles() > 0 );
	RF_ASSERT( mRightWingTileLayer.NumTiles() > 0 );

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	gfx::ppu::Coord const expectedDimensions = CalcContainerDimensions( mSize );
	RF_ASSERT_MSG( container.mAABB.Width() == expectedDimensions.x, "Container not sized as needed" );
	RF_ASSERT_MSG( container.mAABB.Height() == expectedDimensions.y, "Container not sized as needed" );

	// Center elements
	{
		mMainTileLayer.mXCoord = container.mAABB.Left();
		mMainTileLayer.mYCoord = container.mAABB.Top();
		mMainTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
		mMainTileLayer.mLooping = true;
		mMainTileLayer.Animate();

		renderer.DrawTileLayer( mMainTileLayer );
	}

	static constexpr gfx::ppu::Coord kWingDimensions = {
		kWingTileset.mTileWidth,
		kWingTileset.mTileHeight *
			static_cast<gfx::ppu::CoordElem>( character::kMaxSlotsPerElementLevel ) };

	// Left wing elements
	{
		gfx::ppu::AABB wingEnclosure = container.mAABB;
		wingEnclosure -= gfx::ppu::Coord( wingEnclosure.Width(), 0 );
		wingEnclosure -= gfx::ppu::Coord( gfx::ppu::kTileSize / 4, 0 );
		gfx::ppu::Coord const tilePos = AlignToJustify( kWingDimensions, wingEnclosure, Justification::MiddleRight );

		mLeftWingTileLayer.mXCoord = tilePos.x;
		mLeftWingTileLayer.mYCoord = tilePos.y;
		mLeftWingTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
		mLeftWingTileLayer.mLooping = true;
		mLeftWingTileLayer.Animate();

		renderer.DrawTileLayer( mLeftWingTileLayer );
	}

	// Right wing elements
	{
		gfx::ppu::AABB wingEnclosure = container.mAABB;
		wingEnclosure += gfx::ppu::Coord( wingEnclosure.Width(), 0 );
		wingEnclosure += gfx::ppu::Coord( gfx::ppu::kTileSize / 4, 0 );
		gfx::ppu::Coord const tilePos = AlignToJustify( kWingDimensions, wingEnclosure, Justification::MiddleLeft );

		mRightWingTileLayer.mXCoord = tilePos.x;
		mRightWingTileLayer.mYCoord = tilePos.y;
		mRightWingTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
		mRightWingTileLayer.mLooping = true;
		mRightWingTileLayer.Animate();

		renderer.DrawTileLayer( mRightWingTileLayer );
	}

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

	// Center elements
	{
		ElementTilesetDef tilesetDef = {};
		switch( mSize )
		{
			case ElementTileSize::Full:
				tilesetDef = kElementTilesetFull;
				break;
			case ElementTileSize::Medium:
				tilesetDef = kElementTilesetMedium;
				break;
			case ElementTileSize::Micro:
			case ElementTileSize::Mini:
			case ElementTileSize::Invalid:
			default:
				RF_DBGFAIL();
		}

		RF_ASSERT_ASSUME( tilesetDef.mName != nullptr );
		mMainTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( tilesetDef.mName );

		RF_ASSERT( tilesetDef.mUsesBorderSlots == false );
		mMainTileLayer.ClearAndResize( 1, character::kMaxSlotsPerElementLevel );

		RF_ASSERT( tilesetDef.mFont != kInvalidFontPurposeID );
		for( size_t i = 0; i < mNumSlots; i++ )
		{
			WeakPtr<ElementSlotOverlay> const overlay =
				AssignSlotController<ElementSlotOverlay>(
					context, i,
					DefaultCreator<ElementSlotOverlay>::Create(
						mSize ) );
		}
	}

	// Wing elements
	{
		static_assert( kWingTileset.mUsesBorderSlots == false );
		static_assert( kWingTileset.mFont == kInvalidFontPurposeID );

		mLeftWingTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( kWingTileset.mName );
		mRightWingTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( kWingTileset.mName );

		mLeftWingTileLayer.ClearAndResize( 1, character::kMaxSlotsPerElementLevel );
		mRightWingTileLayer.ClearAndResize( 1, character::kMaxSlotsPerElementLevel );
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
		if( isPrevious && mLevel > element::kMinElementLevel )
		{
			mLevel--;
			UpdateDisplay();
			return true;
		}
		else if( isNext && mLevel + 1 <= element::kMaxElementLevel )
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

	static constexpr auto updateTileColumn =
		[]( gfx::ppu::TileLayer& tileLayer, ElementGridDisplayCache::Column const& column ) -> void
	{
		for( size_t i_row = 0; i_row < column.size(); i_row++ )
		{
			ElementGridDisplayCache::Slot const& slot = column.at( i_row );
			tileLayer.GetMutableTile( 0, i_row ).mIndex = math::enum_bitcast( slot.mTilesetIndex );
		}
	};

	static constexpr auto updateTextColumn =
		[]( ElementGridSelector& selector, ElementGridDisplayCache::Column const& column ) -> void
	{
		for( size_t i_row = 0; i_row < column.size(); i_row++ )
		{
			ElementGridDisplayCache::Slot const& slot = column.at( i_row );
			selector.GetMutableSlotController( i_row )->UpdateFromCache( slot );
		}
	};

	// Center elements
	{
		ElementGridDisplayCache::Column const& column = mCache.GetColumnRef( mLevel );
		updateTileColumn( mMainTileLayer, column );
		updateTextColumn( *this, column );
	}

	// Left wing elements
	if( mLevel > element::kMinElementLevel )
	{
		element::ElementLevel const level = math::integer_cast<element::ElementLevel>( mLevel - 1 );
		ElementGridDisplayCache::Column const& column = mCache.GetColumnRef( level );
		updateTileColumn( mLeftWingTileLayer, column );
	}
	else
	{
		mLeftWingTileLayer.Clear();
	}

	// Right wing elements
	if( mLevel < element::kMaxElementLevel )
	{
		element::ElementLevel const level = math::integer_cast<element::ElementLevel>( mLevel + 1 );
		ElementGridDisplayCache::Column const& column = mCache.GetColumnRef( level );
		updateTileColumn( mRightWingTileLayer, column );
	}
	else
	{
		mRightWingTileLayer.Clear();
	}
}

///////////////////////////////////////////////////////////////////////////////
}
