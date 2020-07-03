#include "stdafx.h"
#include "ElementStockpileDisplayCache.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

void ElementStockpileDisplayCache::UpdateFromCompany( state::ObjectRef const& company )
{
	// TODO
	//RF_ASSERT( company.IsSet() );

	mStockpile.resize( 20 );
	size_t i = 0;
	for( Slot& slot : mStockpile )
	{
		slot.mName = "UNSET 00";
		slot.mName.back() += i % 10;
		*( slot.mName.rbegin() + 1 ) += math::integer_cast<uint8_t>( i / 10 );
		i++;

		slot.mTilesetIndex = ElementTilesetIndex::Orange;
	}
}



ElementStockpileDisplayCache::Stockpile const& ElementStockpileDisplayCache::GetStockpileRef() const
{
	return mStockpile;
}

///////////////////////////////////////////////////////////////////////////////
}
