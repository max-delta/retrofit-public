#pragma once
#include "Rollback/Var.h"

#include "core_state/VariableIdentifier.h"
#include "core_allocate/AllocatorFwd.h"

#include "core/meta/FailTemplate.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename T>
class AutoVarBase
{
	RF_NO_COPY( AutoVarBase );

public:
	using VarType = T;

public:
	// NOTE: Will crash if operated on without a valid backing var, default
	//  constructor only here for convenience
	AutoVarBase() = default;

	~AutoVarBase();

	// NOTE: Once bound, the var must outlive the window, or be explicitly
	//  unbound, otherwise it will access invalid memory
	WeakPtr<rollback::Stream<VarType>> Bind(
		Window& window,
		state::VariableIdentifier const& identifier,
		alloc::Allocator& allocator );
	UniquePtr<rollback::Stream<VarType>> Unbind();

protected:
	Var<VarType> mVar;

private:
	Window* mWindow = nullptr;
	state::VariableIdentifier mIdentifier;
};



template<typename T, typename Unused = void>
struct BackingVarType
{
	static_assert( FailTemplate<T>(), "Bad type" );
};
template<typename T>
struct BackingVarType<T, typename rftl::enable_if<rftl::is_enum<T>::value == false>::type>
{
	using Type = T;
};
template<typename T>
struct BackingVarType<T, typename rftl::enable_if<rftl::is_enum<T>::value>::type>
{
	using Type = typename rftl::underlying_type<T>::type;
};

}
///////////////////////////////////////////////////////////////////////////////

// Wrapper for helper Vars that handles automatic cleanup, at the cost of
//  increased memory overhead for this convenience
template<typename T>
class AutoVar : public details::AutoVarBase<typename details::BackingVarType<T>::Type>
{
	RF_NO_COPY( AutoVar );

private:
	using Base = details::AutoVarBase<typename details::BackingVarType<T>::Type>;

public:
	using VarType = typename Base::VarType;
	using Type = T;

public:
	// NOTE: Will crash if operated on without a valid backing var, default
	//  constructor only here for convenience
	AutoVar() = default;
	~AutoVar() = default;

	AutoVar& operator=( Type const& value );
	operator Var<VarType>() const;
	operator Type() const;

	void Write( time::CommonClock::time_point time, Type const& value );
	Type Read( time::CommonClock::time_point time ) const;
	Var<VarType> VarT() const;
	Type As() const;
};

///////////////////////////////////////////////////////////////////////////////
}

extern template class RF::rollback::details::AutoVarBase<bool>;
extern template class RF::rollback::details::AutoVarBase<uint8_t>;
extern template class RF::rollback::details::AutoVarBase<int8_t>;
extern template class RF::rollback::details::AutoVarBase<uint16_t>;
extern template class RF::rollback::details::AutoVarBase<int16_t>;
extern template class RF::rollback::details::AutoVarBase<uint32_t>;
extern template class RF::rollback::details::AutoVarBase<int32_t>;
extern template class RF::rollback::details::AutoVarBase<uint64_t>;
extern template class RF::rollback::details::AutoVarBase<int64_t>;

// NOTE: Many operators in here
#include "AutoVar.inl"
