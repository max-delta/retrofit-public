#pragma once
#include "project.h"

#include "RollbackFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

// Helper class for making state streams act more like normal variables
template<typename T>
class Var
{
public:
	using Type = T;

public:
	// NOTE: Will crash if operated on without a valid backing var, default
	//  constructor only here for convenience
	Var() = default;

	Var( WeakPtr<rollback::Stream<Type>> const& var );
	Var( WeakPtr<rollback::Stream<Type>>&& var );

	Var& operator=( WeakPtr<rollback::Stream<Type>> const& var );
	Var& operator=( WeakPtr<rollback::Stream<Type>>&& var );

	Var& operator=( Type const& value );
	operator Type() const;

	void Write( time::CommonClock::time_point time, Type const& value );
	Type Read( time::CommonClock::time_point time ) const;
	Type As() const;


private:
	WeakPtr<rollback::Stream<Type>> mBackingVar;
};

///////////////////////////////////////////////////////////////////////////////
}}

extern template class RF::rollback::Var<uint8_t>;
extern template class RF::rollback::Var<int8_t>;
extern template class RF::rollback::Var<uint16_t>;
extern template class RF::rollback::Var<int16_t>;
extern template class RF::rollback::Var<uint32_t>;
extern template class RF::rollback::Var<int32_t>;
extern template class RF::rollback::Var<uint64_t>;
extern template class RF::rollback::Var<int64_t>;
