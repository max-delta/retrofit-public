#include "stdafx.h"
#include "ElementGridDisplayCache.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

void ElementGridDisplayCache::UpdateFromCharacter( state::ObjectRef const& character, bool includeStrings )
{
	// TODO
	//RF_ASSERT( character.IsSet() );

	size_t i = 0;
	for( Column& column : mGrid )
	{
		for( Slot& slot : column )
		{
			if( includeStrings == false )
			{
				slot.mName.clear();
			}
			else
			{
				slot.mName = "UNSET 00";
				slot.mName.back() += i % 10;
				*( slot.mName.rbegin() + 1 ) += math::integer_cast<uint8_t>( i / 10 );
				i++;
			}

			slot.mTilesetIndex = ElementTilesetIndex::Purple;
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
