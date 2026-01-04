#include "stdafx.h"
#include "StartupConfig.h"

#include "core_aside/CoffSectionEntry.h"

#include "core/macros.h"


namespace RF::cc::init {
///////////////////////////////////////////////////////////////////////////////
namespace details {

namespace entries {
static constexpr aside::CoffSectionEntry kLOGINFO( "LOGINFO", config::flag::kInformativeLogging );
static constexpr aside::CoffSectionEntry kLOGVERB( "LOGVERB", config::flag::kVerboseLogging );
static constexpr aside::CoffSectionEntry kLOGTRCE( "LOGTRCE", config::flag::kTraceLogging );
}

namespace data {
RF_DECLARE_SECTION_RW( "RFCFG" );
RF_ASSIGN_TO_SECTION( "RFCFG" )
static aside::CoffSectionEntry sRFCFG[] = {
	aside::CoffSectionEntry( "RFCFG==", 0x1122334455667788ull ),
	entries::kLOGINFO,
	entries::kLOGVERB,
	entries::kLOGTRCE,
	aside::CoffSectionEntry( "RFCFG^^", 0xFFFFFFFFFFFFFFFFull ),
};
}
static constexpr rftl::span<aside::CoffSectionEntry> kRFCFG( data::sRFCFG );
static_assert( kRFCFG.size() == 5 );
static_assert( sizeof( decltype( data::sRFCFG ) ) == 16 * kRFCFG.size() );
static_assert( alignof( decltype( data::sRFCFG ) ) == 16 );

}
///////////////////////////////////////////////////////////////////////////////

StartupConfig StartupConfig::FetchFromBinary()
{
	StartupConfig retVal = {};

	static constexpr auto getOrAssert = []( rftl::string_view name ) -> bool
	{
		aside::CoffSectionEntry const entry =
			aside::CoffSectionEntry::FindEntry(
				details::kRFCFG,
				name );
		RF_ASSERT( entry.GetName() == name );
		return entry.AsBool();
	};

	// Hard-coded
	{
		retVal.mWindowTitle = "CC3O3 Pre-Alpha";
	}

	// Entries
	{
		using namespace details::entries;
		retVal.mInformativeLogging = getOrAssert( kLOGINFO.GetName() );
		retVal.mVerboseLogging = getOrAssert( kLOGVERB.GetName() );
		retVal.mTraceLogging = getOrAssert( kLOGTRCE.GetName() );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
