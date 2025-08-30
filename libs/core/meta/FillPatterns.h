#pragma once

#include "rftl/cstdint"


namespace RF::meta::fill {
///////////////////////////////////////////////////////////////////////////////
namespace details {
// To be a NaN, the exponent must be all ones (B), the sign bit can be
//  anything (A), the highest fraction bit should be zero to make it a
//  signalling NaN (C), and the fraction must be non-zero (D)
static constexpr uint32_t kNanPartialExponentMask4Bytes =
	//A BBBBBBBB C'DDDDDDDDDDDDDDDDDDDDD
	0b0'11111111'0'0000000000000000000000;

// To be non-ASCII, each byte must have the upper bit set
static constexpr uint32_t kNonASCIIMask4Bytes = 0b10000000'10000000'10000000'10000000;

// In Windows, pointers with the top bit set are reserved for kernel addresses
static constexpr uint32_t kBadWin32PointerMask4Bytes = 0b10000000'00000000'00000000'00000000;

// Try to make it stand out more to human readers
static constexpr uint32_t kHumanFudgeMask4Bytes = 0x00'bbbbbb;

// An ideal bad fill pattern will cause issues in several different ways of
//  interpreting it, but also be quickly recognizable to a human in hexadecimal
static constexpr uint32_t kAlignedBad4Bytes =
	kNanPartialExponentMask4Bytes |
	kBadWin32PointerMask4Bytes |
	kNonASCIIMask4Bytes |
	kHumanFudgeMask4Bytes;
static_assert( kAlignedBad4Bytes == 0b1'11111111'0'1110111011101110111011 );
static_assert( kAlignedBad4Bytes == 0xffbbbbbb );

// A good 4-byte pattern is even better if it is still bad when offset slightly
static constexpr uint32_t kUnalignedBad4Bytes = kAlignedBad4Bytes >> 16 | kAlignedBad4Bytes;
static_assert( kUnalignedBad4Bytes == 0xffbbffbb );
}
///////////////////////////////////////////////////////////////////////////////

static constexpr uint32_t kBad4Bytes = details::kUnalignedBad4Bytes;
static_assert( kBad4Bytes == 0xffbb'ffbb );

static constexpr uint64_t kBad8Bytes = static_cast<uint64_t>( kBad4Bytes ) << 32 | kBad4Bytes;
static_assert( kBad8Bytes == 0xffbb'ffbb'ffbb'ffbb );

static constexpr uint16_t kBad2Bytes = kBad4Bytes >> 16;
static_assert( kBad2Bytes == 0xffbb );

///////////////////////////////////////////////////////////////////////////////
}
