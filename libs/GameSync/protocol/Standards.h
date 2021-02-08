#pragma once
#include "project.h"

#include "rftl/chrono"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

#if 1 // PARANOID
// The logic here is that IPv4 should be the most limiting transport in
//  practice, with a 576-byte internet packet being the smallest practical
//  packet size worth supporting.
//  +576 base packet
//  -60 IP header
//  -8 UDP header
//  -8 Reliability header
static constexpr size_t kMaxRecommendedTransmissionSize = 500;
#else // NORMAL
// The logic here is that in practice, the majority of internet routes can
//  survive packets that approach 1,500 bytes. Staying well clear of that
//  should still give plenty of flexibility.
static constexpr size_t kMaxRecommendedTransmissionSize = 1200;
#endif



#if RF_IS_ALLOWED( RF_CONFIG_LONG_TIMEOUTS )
static constexpr rftl::chrono::hours kRecommendedHandshakeTimeout{ 24 };
#else
static constexpr rftl::chrono::seconds kRecommendedHandshakeTimeout{ 5 };
#endif

///////////////////////////////////////////////////////////////////////////////
}
