#include "stdafx.h"
#include "ElementGridSelector.h"

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

ElementGridSelector::ElementGridSelector()
	: GenericListBox( character::kMaxSlotsPerElementLevel )
{
	//
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



void ElementGridSelector::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
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

void ElementGridSelector::PostInstanceAssign( UIContext& context, Container& container )
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



WeakPtr<TextLabel> ElementGridSelector::GetMutableSlotController( size_t slotIndex )
{
	WeakPtr<TextLabel> retVal;
	PtrTransformer<TextLabel>::PerformNonTypesafeTransformation(
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
		GetMutableSlotController( i_row )->SetText( slot.mName );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
