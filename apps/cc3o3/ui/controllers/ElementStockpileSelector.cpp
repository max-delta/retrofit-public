#include "stdafx.h"
#include "ElementStockpileSelector.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusEvent.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::ElementStockpileSelector )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::ElementStockpileSelector );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

ElementStockpileSelector::ElementStockpileSelector()
	: GenericListBox( character::kMaxSlotsPerElementLevel )
{
	//
}



void ElementStockpileSelector::UpdateFromCompany( state::ObjectRef const& company )
{
	mCache.UpdateFromCompany( company );
	UpdateDisplay();
}



void ElementStockpileSelector::UpdateFromCache( ElementStockpileDisplayCache const& cache )
{
	mCache = cache;
	UpdateDisplay();
}



element::ElementIdentifier ElementStockpileSelector::GetSelectedIdentifier( UIConstContext const& context ) const
{
	size_t const displayOffset = GetSlotIndexWithSoftFocus( context );
	size_t const cacheOffset = mListOffset + displayOffset;
	return mCache.GetStockpileRef().at( cacheOffset ).mIdentifier;
}



void ElementStockpileSelector::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	RF_ASSERT_MSG( container.mAABB.Width() == kContainerWidth, "Container not sized as needed" );
	RF_ASSERT_MSG( container.mAABB.Height() == kContainerHeight, "Container not sized as needed" );
	RF_ASSERT( mTileLayer.NumTiles() > 0 );

	gfx::ppu::PPUController& renderer = GetRenderer( context.GetContainerManager() );

	mTileLayer.mXCoord = container.mAABB.Left();
	mTileLayer.mYCoord = container.mAABB.Top();
	mTileLayer.mZLayer = context.GetContainerManager().GetRecommendedRenderDepth( container );
	mTileLayer.mLooping = true;
	mTileLayer.Animate();

	renderer.DrawTileLayer( mTileLayer );
}

///////////////////////////////////////////////////////////////////////////////

void ElementStockpileSelector::PostInstanceAssign( UIContext& context, Container& container )
{
	gfx::ppu::PPUController const& renderer = GetRenderer( context.GetContainerManager() );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

	mTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( kElementTilesetDef.mName );
	mTileLayer.ClearAndResize( 1, character::kMaxSlotsPerElementLevel );

	for( size_t i = 0; i < mNumSlots; i++ )
	{
		WeakPtr<TextLabel> const label = AssignSlotController<TextLabel>( context, i, DefaultCreator<TextLabel>::Create() );
		label->SetJustification( ui::Justification::MiddleCenter );
		label->SetFont( ui::font::LargeMenuText );
		label->SetText( "UNSET" );
		label->SetColor( math::Color3f::kWhite );
		label->SetBorder( true );
	}
}



bool ElementStockpileSelector::ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const
{
	// TODO
	return false;
}



bool ElementStockpileSelector::OnUnhandledFocusEvent( UIContext& context, FocusEvent const& focusEvent )
{
	bool const isPrevious = focusEvent.mEventType == focusevent::Command_NavigateUp;
	bool const isNext = focusEvent.mEventType == focusevent::Command_NavigateDown;
	bool const isPageUp = mPagination && focusEvent.mEventType == focusevent::Command_NavigateToPreviousGroup;
	bool const isPageDown = mPagination && focusEvent.mEventType == focusevent::Command_NavigateToNextGroup;
	bool const isFirst = focusEvent.mEventType == focusevent::Command_NavigateToFirst;
	bool const isLast = focusEvent.mEventType == focusevent::Command_NavigateToLast;
	bool const isIncremental = isPrevious || isNext || isPageUp || isPageDown;
	bool const isTargeted = isFirst || isLast;
	bool const isCycle = isIncremental || isTargeted;

	if( isCycle )
	{
		size_t const stockpileSize = mCache.GetStockpileRef().size();

		if( isPrevious && mListOffset > 0 )
		{
			mListOffset--;
			UpdateDisplay();
			return true;
		}
		else if( isPageUp )
		{
			if( mListOffset >= mNumSlots )
			{
				mListOffset -= mNumSlots;
				UpdateDisplay();
				return true;
			}
			else
			{
				// Snap to begin
				mListOffset = 0;
				UpdateDisplay();
				return true;
			}
		}
		else if( isFirst )
		{
			mListOffset = 0;
			UpdateDisplay();
			return true;
		}
		else if( isNext && mListOffset + mNumSlots < stockpileSize )
		{
			mListOffset++;
			UpdateDisplay();
			return true;
		}
		else if( isPageDown )
		{
			if( mListOffset + mNumSlots + ( mNumSlots - 1 ) < stockpileSize )
			{
				mListOffset += mNumSlots;
				UpdateDisplay();
				return true;
			}
			else
			{
				// Snap to end
				mListOffset = stockpileSize - mNumSlots;
				UpdateDisplay();
				return true;
			}
		}
		else if( isLast )
		{
			mListOffset = stockpileSize - mNumSlots;
			UpdateDisplay();
			return true;
		}

		// Sink any attempts to move out of selector
		// TODO: Configurable?
		return true;
	}

	return false;
}



WeakPtr<TextLabel> ElementStockpileSelector::GetMutableSlotController( size_t slotIndex )
{
	WeakPtr<TextLabel> retVal;
	PtrTransformer<TextLabel>::PerformNonTypesafeTransformation(
		GenericListBox::GetMutableSlotController( slotIndex ), retVal );
	return retVal;
}



void ElementStockpileSelector::UpdateDisplay()
{
	// TODO: Sanitize list offset if it would be past the end of the list for
	//  the current focus

	ElementStockpileDisplayCache::Stockpile const& stockpile = mCache.GetStockpileRef();
	for( size_t i_slot = 0; i_slot < mNumSlots; i_slot++ )
	{
		ElementStockpileDisplayCache::Slot const& slot = stockpile.at( mListOffset + i_slot );
		mTileLayer.GetMutableTile( 0, i_slot ).mIndex = math::enum_bitcast( slot.mTilesetIndex );
		GetMutableSlotController( i_slot )->SetText( slot.mName );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
