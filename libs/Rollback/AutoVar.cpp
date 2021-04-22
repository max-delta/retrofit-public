#include "stdafx.h"
#include "AutoVar.h"

#include "Timing/FrameClock.h"

#include "core_state/StateBag.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename T>
AutoVarBase<T>::~AutoVarBase()
{
	if( mWindow != nullptr )
	{
		UniquePtr<rollback::Stream<VarType>> const ref = Unbind();

		// The expected use case of the AutoVar is for it to be used instead of
		//  streams. In such a scenario, it should be the only reference to a
		//  given stream. Having multiple AutoVar's pointing to the same
		//  stream, for example, can cause this assert to fail
		RF_ASSERT_MSG( ref.IsOnlyWeakReference(), "AutoVar destroying stream with weak references" );
	}
}



template<typename T>
WeakPtr<rollback::Stream<typename AutoVarBase<T>::VarType>> AutoVarBase<T>::Bind(
	Window& window,
	state::VariableIdentifier const& identifier,
	alloc::Allocator& allocator )
{
	RF_ASSERT( mWindow == nullptr );
	mWindow = &window;
	mIdentifier = identifier;
	WeakPtr<rollback::Stream<VarType>> const retVal = window.GetOrCreateStream<VarType>( mIdentifier, allocator );
	RF_ASSERT( retVal != nullptr );
	mVar = retVal;
	return retVal;
}



template<typename T>
UniquePtr<rollback::Stream<typename AutoVarBase<T>::VarType>> AutoVarBase<T>::Unbind()
{
	RF_ASSERT( mWindow != nullptr );
	UniquePtr<rollback::Stream<VarType>> retVal = mWindow->RemoveStream<VarType>( mIdentifier );
	RF_ASSERT( retVal != nullptr );
	mWindow = nullptr;
	mIdentifier.clear();
	mVar = Var<VarType>{ nullptr };
	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////
}

static_assert( RF::state::StateBag<1>::ContainedTypes::kNumTypes == 9, "Unexpected size" );
template class ROLLBACK_API RF::rollback::details::AutoVarBase<bool>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<uint8_t>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<int8_t>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<uint16_t>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<int16_t>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<uint32_t>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<int32_t>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<uint64_t>;
template class ROLLBACK_API RF::rollback::details::AutoVarBase<int64_t>;
