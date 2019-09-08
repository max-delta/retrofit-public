#pragma once
#include "core_math/math_bits.h"

#include "rftl/cstddef"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

// The maximum alignment is generally for 'double' or 'long double'
constexpr size_t kMaxAlign = alignof( rftl::max_align_t );
static_assert( kMaxAlign >= 4, "Unreasonably small alignment" );
static_assert( kMaxAlign <= 16, "Unexpectedly large alignment" );

// The C++ standard specifies that all alignments are powers of two, and that
//  there is a max alignment that is not overly large, so that all allocators
//  can practically ensure alignment is always met by always aligning to the
//  maximum value
// NOTE: There are the concepts of 'over-alignment' and 'extended alignment'
//  in the standard, but these are implementation-dependent and thus ignored
//  here for portability reasons
constexpr bool IsValidAlignment( size_t const align )
{
	return math::IsPowerOfTwo( align ) && align <= kMaxAlign;
}

///////////////////////////////////////////////////////////////////////////////
}}
