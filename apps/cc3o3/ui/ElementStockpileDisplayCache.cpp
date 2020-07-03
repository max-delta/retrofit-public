#include "stdafx.h"
#include "ElementStockpileDisplayCache.h"

#include "cc3o3/Common.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/ElementDatabase.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

void ElementStockpileDisplayCache::UpdateFromCompany( state::ObjectRef const& company )
{
	using ElementCount = rftl::pair<element::ElementIdentifier, size_t>;
	using ElementCounts = rftl::vector<ElementCount>;
	ElementCounts sortedElements;
	{
		company::CompanyManager::ElementCounts const elementCounts = gCompanyManager->CalcTotalElements( company );
		// TODO: Reduce counts by amounts equipped by active party

		sortedElements.assign( elementCounts.begin(), elementCounts.end() );
		// TODO: Sorting/filtering
	}

	size_t const numSlots = sortedElements.size();
	mStockpile.resize( numSlots );
	for( size_t i = 0; i < numSlots; i++ )
	{
		Slot& slot = mStockpile.at( i );
		ElementCount const& countEntry = sortedElements.at( i );
		element::ElementDesc const desc = gElementDatabase->GetElementDesc( countEntry.first );
		slot.UpdateFromDesc( desc, true );
	}
}



ElementStockpileDisplayCache::Stockpile const& ElementStockpileDisplayCache::GetStockpileRef() const
{
	return mStockpile;
}

///////////////////////////////////////////////////////////////////////////////
}
