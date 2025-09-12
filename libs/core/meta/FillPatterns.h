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

// An ideal bad fill pattern will cause issues in several different ways of
//  interpreting it
static constexpr uint32_t kBasePatternBad4Bytes =
	kNanPartialExponentMask4Bytes |
	kBadWin32PointerMask4Bytes |
	kNonASCIIMask4Bytes;
static_assert( kBasePatternBad4Bytes == 0b1'11111111'0'0000001000000010000000 );
static_assert( kBasePatternBad4Bytes == 0xff808080 );

// A good 4-byte pattern is even better if it is still bad when offset slightly
static constexpr uint32_t kUnalignedBaseBad4Bytes = kBasePatternBad4Bytes >> 16 | kBasePatternBad4Bytes;
static_assert( kUnalignedBaseBad4Bytes == 0xff80ff80 );

// Try to make it stand out more to human readers, and provide variants
static constexpr uint32_t kHumanFudgeMask4Bytes_0 = 0x00'00'00'00;
static constexpr uint32_t kHumanFudgeMask4Bytes_A = 0x00'aa'00'aa;
static constexpr uint32_t kHumanFudgeMask4Bytes_B = 0x00'bb'00'bb;
static constexpr uint32_t kHumanFudgeMask4Bytes_8 = 0x00'88'00'88;
static constexpr uint32_t kHumanFudgeMask4Bytes_9 = 0x00'99'00'99;

// The final patterns
static constexpr uint32_t kBadPattern4_0 = kUnalignedBaseBad4Bytes | kHumanFudgeMask4Bytes_0;
static constexpr uint32_t kBadPattern4_A = kUnalignedBaseBad4Bytes | kHumanFudgeMask4Bytes_A;
static constexpr uint32_t kBadPattern4_B = kUnalignedBaseBad4Bytes | kHumanFudgeMask4Bytes_B;
static constexpr uint32_t kBadPattern4_8 = kUnalignedBaseBad4Bytes | kHumanFudgeMask4Bytes_8;
static constexpr uint32_t kBadPattern4_9 = kUnalignedBaseBad4Bytes | kHumanFudgeMask4Bytes_9;
static_assert( kBadPattern4_0 == 0xff80ff80 );
static_assert( kBadPattern4_A == 0xffAAffAA );
static_assert( kBadPattern4_B == 0xffBBffBB );
static_assert( kBadPattern4_8 == 0xff88ff88 );
static_assert( kBadPattern4_9 == 0xff99ff99 );

}
///////////////////////////////////////////////////////////////////////////////

static constexpr uint16_t kBadPattern2_0 = details::kBadPattern4_0 >> 16;
static constexpr uint16_t kBadPattern2_A = details::kBadPattern4_A >> 16;
static constexpr uint16_t kBadPattern2_B = details::kBadPattern4_B >> 16;
static constexpr uint16_t kBadPattern2_8 = details::kBadPattern4_8 >> 16;
static constexpr uint16_t kBadPattern2_9 = details::kBadPattern4_9 >> 16;

static constexpr uint32_t kBadPattern4_0 = details::kBadPattern4_0;
static constexpr uint32_t kBadPattern4_A = details::kBadPattern4_A;
static constexpr uint32_t kBadPattern4_B = details::kBadPattern4_B;
static constexpr uint32_t kBadPattern4_8 = details::kBadPattern4_8;
static constexpr uint32_t kBadPattern4_9 = details::kBadPattern4_9;

static constexpr uint64_t kBadPattern8_0 = static_cast<uint64_t>( details::kBadPattern4_0 ) << 32 | details::kBadPattern4_0;
static constexpr uint64_t kBadPattern8_A = static_cast<uint64_t>( details::kBadPattern4_A ) << 32 | details::kBadPattern4_A;
static constexpr uint64_t kBadPattern8_B = static_cast<uint64_t>( details::kBadPattern4_B ) << 32 | details::kBadPattern4_B;
static constexpr uint64_t kBadPattern8_8 = static_cast<uint64_t>( details::kBadPattern4_8 ) << 32 | details::kBadPattern4_8;
static constexpr uint64_t kBadPattern8_9 = static_cast<uint64_t>( details::kBadPattern4_9 ) << 32 | details::kBadPattern4_9;

///////////////////////////////////////////////////////////////////////////////
}
