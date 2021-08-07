#pragma once
#include "rftl/cstdint"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// This random function will not change based on platform
// NOTE: Not a 'good' generator, but good enough for gameplay code
constexpr uint32_t kStableRandLCGMaxEntropy = 2'147'483'647;
uint32_t StableRandLCG( uint32_t previousValue );
uint8_t StableRandLCGPercent( uint32_t& lastGeneratorValue );
bool StableRandLCGPercentAbove( uint8_t percentThreshold, uint32_t& lastGeneratorValue );
bool StableRandLCGPercentBelow( uint8_t percentThreshold, uint32_t& lastGeneratorValue );

uint32_t GetSeedFromHash( uint64_t hash );

///////////////////////////////////////////////////////////////////////////////
}
