#include "stdafx.h"
#include "Exfiltrate.h"

#include "Timing/FrameClock.h"

#include "core_aside/CoffSectionEntry.h"
#include "core_math/math_casts.h"

#include "core/macros.h"


namespace RF::cc::exfil {
///////////////////////////////////////////////////////////////////////////////
namespace details {

namespace entries {
static constexpr aside::CoffSectionEntry kFRMCLCK( "FRMCLCK", 0ull );
}

namespace data {
RF_DECLARE_SECTION_RW( "RFXFL" );
RF_ASSIGN_TO_SECTION( "RFXFL" )
static aside::CoffSectionEntry sRFXFL[] = {
	aside::CoffSectionEntry( "RFXFL==", 0x1122334455667788ull ),
	entries::kFRMCLCK,
	aside::CoffSectionEntry( "RFXFL^^", 0xFFFFFFFFFFFFFFFFull ),
};
}
static constexpr rftl::span<aside::CoffSectionEntry> kRFXFL( data::sRFXFL );
static_assert( kRFXFL.size() == 3 );
static_assert( sizeof( decltype( data::sRFXFL ) ) == 16 * kRFXFL.size() );
static_assert( alignof( decltype( data::sRFXFL ) ) == 16 );

}
///////////////////////////////////////////////////////////////////////////////

void ExfiltrateFrameData()
{
	// What time does the game think it is? This is notably different than
	//  wall-clock time in several significant ways:
	//  * It is strictly controlled by the game's simulation logic, so won't be
	//     affected by hitches, performance issues, or intentionally induced
	//     slowdowns as part of network clock synchronization with laggy peers
	//  * It responds to save-state / load-state and rewind logic
	static aside::CoffSectionEntry& frameClock =
		*aside::CoffSectionEntry::FindMutableEntryRef(
			details::kRFXFL,
			details::entries::kFRMCLCK.GetName() );
	frameClock.WriteUInt64(
		math::integer_cast<uint64_t>(
			time::FrameClock::now().time_since_epoch().count() ) );
}

///////////////////////////////////////////////////////////////////////////////
}
