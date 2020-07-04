#include "stdafx.h"
#include "ElementStockpileDisplayCache.h"

#include "cc3o3/Common.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/ElementDatabase.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "rftl/algorithm"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

void ElementStockpileDisplayCache::UpdateFromCompany( state::ObjectRef const& company )
{
	using ElementCount = rftl::pair<element::ElementDesc, size_t>;
	using ElementCounts = rftl::vector<ElementCount>;
	ElementCounts sortedElements;
	{
		// Calculate
		company::CompanyManager::ElementCounts const elementCounts = gCompanyManager->CalcTotalElements( company );
		// TODO: Reduce counts by amounts equipped by active party

		// Lookup
		for( company::CompanyManager::ElementCounts::value_type const& entry : elementCounts )
		{
			element::ElementDesc desc = gElementDatabase->GetElementDesc( entry.first );
			sortedElements.emplace_back( rftl::move( desc ), entry.second );
		}

		// Sort/filter
		// TODO: Options
		static constexpr auto sortLevel = []( ElementCount const& lhs, ElementCount const& rhs ) -> bool //
		{
			return element::SortPredicateDescLevel( lhs.first, rhs.first );
		};
		static constexpr auto sortInnate = []( ElementCount const& lhs, ElementCount const& rhs ) -> bool //
		{
			return element::SortPredicateDescInnate( lhs.first, rhs.first );
		};
		static constexpr auto filterNone = []( ElementCount const& val ) -> bool //
		{
			return false;
		};
		rftl::stable_sort( sortedElements.begin(), sortedElements.end(), sortLevel );
		rftl::stable_sort( sortedElements.begin(), sortedElements.end(), sortInnate );
		sortedElements.erase( rftl::remove_if( sortedElements.begin(), sortedElements.end(), filterNone ), sortedElements.end() );
	}

	size_t const numSlots = sortedElements.size();
	mStockpile.resize( numSlots );
	for( size_t i = 0; i < numSlots; i++ )
	{
		Slot& slot = mStockpile.at( i );
		ElementCount const& elemEntry = sortedElements.at( i );
		slot.UpdateFromDesc( elemEntry.first, true );
	}
}



ElementStockpileDisplayCache::Stockpile const& ElementStockpileDisplayCache::GetStockpileRef() const
{
	return mStockpile;
}

///////////////////////////////////////////////////////////////////////////////
}
