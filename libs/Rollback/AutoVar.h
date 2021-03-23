#pragma once
#include "Rollback/Var.h"

#include "core_state/VariableIdentifier.h"

#include "core_allocate/AllocatorFwd.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

// Wrapper for helper Vars that handles automatic cleanup, at the cost of
//  increased memory overhead for this convenience
template<typename T>
class AutoVar
{
	RF_NO_COPY( AutoVar );

public:
	using Type = T;

public:
	// NOTE: Will crash if operated on without a valid backing var, default
	//  constructor only here for convenience
	AutoVar() = default;

	~AutoVar();

	AutoVar& operator=( Type const& value );
	operator Var<T>() const;
	operator Type() const;

	void Write( time::CommonClock::time_point time, Type const& value );
	Type Read( time::CommonClock::time_point time ) const;
	Var<T> VarT() const;
	Type As() const;


	// NOTE: Once bound, the var must outlive the window, or be explicitly
	//  unbound, otherwise it will access invalid memory
	WeakPtr<rollback::Stream<Type>> Bind( Window& window, state::VariableIdentifier const& identifier, alloc::Allocator& allocator );
	UniquePtr<rollback::Stream<Type>> Unbind();


private:
	Var<T> mVar;
	Window* mWindow = nullptr;
	state::VariableIdentifier mIdentifier;
};

///////////////////////////////////////////////////////////////////////////////
}

extern template class RF::rollback::AutoVar<bool>;
extern template class RF::rollback::AutoVar<uint8_t>;
extern template class RF::rollback::AutoVar<int8_t>;
extern template class RF::rollback::AutoVar<uint16_t>;
extern template class RF::rollback::AutoVar<int16_t>;
extern template class RF::rollback::AutoVar<uint32_t>;
extern template class RF::rollback::AutoVar<int32_t>;
extern template class RF::rollback::AutoVar<uint64_t>;
extern template class RF::rollback::AutoVar<int64_t>;

// NOTE: Many operators in here
#include "AutoVar.inl"
