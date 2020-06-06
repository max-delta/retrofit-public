#include "stdafx.h"
#include "ElementGridDisplayCache.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

void ElementGridDisplayCache::UpdateFromCharacter( state::ObjectRef const& character, bool includeStrings )
{
	// TODO
	//RF_ASSERT( character.IsSet() );

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
				slot.mName = "UNSET";
			}

			slot.mTilesetIndex = ElementTilesetIndex::Invalid;
		}
	}
}



ElementGridDisplayCache::Grid const& ElementGridDisplayCache::GetGridRef() const
{
	return mGrid;
}

///////////////////////////////////////////////////////////////////////////////
}
