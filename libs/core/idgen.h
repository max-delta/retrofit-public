#pragma once

#include "core/macros.h"

#include "rftl/type_traits"
#include "rftl/limits"
#include "rftl/atomic"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename BackingType>
class NonloopingIDGenerator
{
	static_assert( rftl::is_integral<BackingType>::value, "Only integral types supported" );

	RF_NO_COPY( NonloopingIDGenerator );

	//
	// Types and constants
public:
	using ID = BackingType;
	static constexpr ID kInvalid = rftl::numeric_limits<ID>::min();
	static constexpr ID kInitial = kInvalid + 1;
	static_assert( kInvalid != kInitial, "Unexpected incrementation behavior" );
	static constexpr ID kFinal = rftl::numeric_limits<ID>::max() - 1u;

private:
	using Storage = rftl::atomic<ID>;
	static constexpr bool kRolloverUnrealistic =
		kInitial >= 0 &&
		kInitial <= rftl::numeric_limits<int8_t>::max() &&
		kFinal >= rftl::numeric_limits<int64_t>::max();


	//
	// Public methods
public:
	NonloopingIDGenerator() = default;

	// Thread-safe
	ID Generate()
	{
		if( kRolloverUnrealistic )
		{
			// Can't realistically overflow, statically avoid checking for
			//  rollover behavior
		}
		else
		{
			// May be able to rollover, check and prevent that
			ID const currentID = mStorage.load( rftl::memory_order::memory_order_acquire );
			static_assert( kFinal + 1 > kFinal, "Unexpected rollover" );
			if( currentID > kFinal )
			{
				RF_DBGFAIL_MSG( "Prevented ID generation roll-over" );
				return kInvalid;
			}
		}

		ID const newID = mStorage.fetch_add( 1, rftl::memory_order::memory_order_acq_rel );
		RF_ASSERT_MSG( newID != kInvalid, "Invalid ID generation, probabaly from roll-over" );
		return newID;
	}

	// Thread-safe
	ID GetLastGeneratedID() const
	{
		ID const nextID = mStorage.load( rftl::memory_order::memory_order_acquire );
		RF_ASSERT_MSG( nextID > kInvalid, "Invalid ID generation, probabaly from roll-over" );
		return nextID - 1;
	}


	//
	// Private data
private:
	Storage mStorage = kInitial;
};

///////////////////////////////////////////////////////////////////////////////
}
