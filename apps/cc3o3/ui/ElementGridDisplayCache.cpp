#include "stdafx.h"
#include "ElementGridDisplayCache.h"

#include "cc3o3/Common.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Loadout.h"
#include "cc3o3/elements/ElementDatabase.h"

#include "core_component/TypedObjectRef.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

void ElementGridDisplayCache::UpdateFromCharacter( state::ObjectRef const& character, bool includeStrings )
{
	RF_ASSERT( character.IsSet() );

	WeakPtr<state::comp::Loadout const> const loadout = character.GetComponentInstanceT<state::comp::Loadout>();
	RF_ASSERT( loadout != nullptr );

	mGridSizeMask = loadout->mGridSizeMask;

	character::ElementSlots::Grid const sourceGrid = loadout->CalcElements().mGrid;
	static_assert( rftl::tuple_size<character::ElementSlots::Grid>::value == rftl::tuple_size<Grid>::value );
	static_assert( rftl::tuple_size<character::ElementSlots::Column>::value == rftl::tuple_size<Column>::value );

	element::ElementDatabase const& elemDB = *gElementDatabase;

	// For each column / level...
	for( size_t i_col = 0; i_col < mGrid.size(); i_col++ )
	{
		Column& destColumn = mGrid.at( i_col );
		character::ElementSlots::Column const& sourceColumn = sourceGrid.at( i_col );

		element::ElementLevel const elementLevel = element::FromLevelOffset( i_col );
		size_t const numUnmaskedSlots = mGridSizeMask.GetNumSlotsAtLevel( elementLevel );

		// For each slot...
		for( size_t i_slot = 0; i_slot < destColumn.size(); i_slot++ )
		{
			Slot& destSlot = destColumn.at( i_slot );
			character::ElementSlots::Slot const& sourceSlot = sourceColumn.at( i_slot );

			bool const isOutsideOfMask = i_slot >= numUnmaskedSlots;

			element::ElementIdentifier const identifier = sourceSlot;
			if( identifier == element::kInvalidElementIdentifier )
			{
				if( isOutsideOfMask )
				{
					// Can't be assigned
					destSlot.MaskOut();
				}
				else
				{
					// Is not assigned
					destSlot.Unallocate();
				}
			}
			else
			{
				// Is assigned
				// NOTE: Could still be out of the mask, but that's considered a
				//  potential validation problem, and outside the scope of this
				//  display logic
				element::ElementDesc const desc = elemDB.GetElementDesc( identifier );
				destSlot.UpdateFromDesc( desc, includeStrings );
			}
		}
	}
}



void ElementGridDisplayCache::DarkenAll()
{
	for( Column& column : mGrid )
	{
		for( Slot& slot : column )
		{
			slot.Darken();
		}
	}
}



ElementGridDisplayCache::Slot const& ElementGridDisplayCache::GetSlotRef( character::ElementSlotIndex const& location ) const
{
	return GetColumnRef( location.first ).at( location.second );
}



ElementGridDisplayCache::Column const& ElementGridDisplayCache::GetColumnRef( element::ElementLevel const& level ) const
{
	size_t const levelOffset = element::AsLevelOffset( level );
	return mGrid.at( levelOffset );
}

///////////////////////////////////////////////////////////////////////////////
}
