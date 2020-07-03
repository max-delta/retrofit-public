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



void ElementStockpileSelector::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	RF_ASSERT_MSG( container.mAABB.Width() == kContainerWidth, "Container not sized as needed" );
	RF_ASSERT_MSG( container.mAABB.Height() == kContainerHeight, "Container not sized as needed" );
	RF_ASSERT( mTileLayer.NumTiles() > 0 );

	gfx::PPUController& renderer = GetRenderer( context.GetContainerManager() );

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
	gfx::PPUController const& renderer = GetRenderer( context.GetContainerManager() );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

	mTileLayer.mTilesetReference = tsetMan.GetManagedResourceIDFromResourceName( kElementTilesetFullName );
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
	bool const isCycle = isPrevious || isNext;

	if( isCycle )
	{
		if( isPrevious && mListOffset > 0 )
		{
			mListOffset--;
			UpdateDisplay();
			return true;
		}
		else if( isNext && mListOffset + 1 < mCache.GetStockpileRef().size() )
		{
			mListOffset++;
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
