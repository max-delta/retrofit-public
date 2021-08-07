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

	character::ElementSlots::Grid const& sourceGrid = loadout->mEquippedElements.mGrid;
	static_assert( rftl::tuple_size<character::ElementSlots::Grid>::value == rftl::tuple_size<Grid>::value );
	static_assert( rftl::tuple_size<character::ElementSlots::Column>::value == rftl::tuple_size<Column>::value );

	element::ElementDatabase const& elemDB = *gElementDatabase;

	for( size_t i_col = 0; i_col < mGrid.size(); i_col++ )
	{
		Column& destColumn = mGrid.at( i_col );
		character::ElementSlots::Column const& sourceColumn = sourceGrid.at( i_col );

		for( size_t i_slot = 0; i_slot < destColumn.size(); i_slot++ )
		{
			Slot& destSlot = destColumn.at( i_slot );
			character::ElementSlots::Slot const& sourceSlot = sourceColumn.at( i_slot );

			element::ElementIdentifier const identifier = sourceSlot;
			if( identifier == element::kInvalidElementIdentifier )
			{
				destSlot.Unallocate();
			}
			else
			{
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



ElementGridDisplayCache::Grid const& ElementGridDisplayCache::GetGridRef() const
{
	return mGrid;
}

///////////////////////////////////////////////////////////////////////////////
}
