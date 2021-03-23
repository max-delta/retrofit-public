#include "stdafx.h"
#include "AutoVar.h"

#include "Timing/FrameClock.h"

#include "core_state/StateBag.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
AutoVar<T>::~AutoVar()
{
	if( mWindow != nullptr )
	{
		UniquePtr<rollback::Stream<Type>> const ref = Unbind();

		// The expected use case of the AutoVar is for it to be used instead of
		//  streams. In such a scenario, it should be the only reference to a
		//  given stream. Having multiple AutoVar's pointing to the same
		//  stream, for example, can cause this assert to fail
		RF_ASSERT_MSG( ref.IsOnlyWeakReference(), "AutoVar destroying stream with weak references" );
	}
}



template<typename T>
AutoVar<T>& AutoVar<T>::operator=( Type const& value )
{
	mVar.operator=( value );
	return *this;
}



template<typename T>
AutoVar<T>::operator Var<T>() const
{
	return mVar;
}



template<typename T>
AutoVar<T>::operator Type() const
{
	return mVar.operator Type();
}



template<typename T>
void AutoVar<T>::Write( time::CommonClock::time_point time, Type const& value )
{
	mVar.Write( time, value );
}



template<typename T>
typename AutoVar<T>::Type AutoVar<T>::Read( time::CommonClock::time_point time ) const
{
	return mVar.Read( time );
}



template<typename T>
Var<T> AutoVar<T>::VarT() const
{
	return mVar;
}



template<typename T>
typename AutoVar<T>::Type AutoVar<T>::As() const
{
	return mVar.As();
}



template<typename T>
WeakPtr<rollback::Stream<typename AutoVar<T>::Type>> AutoVar<T>::Bind( Window& window, state::VariableIdentifier const& identifier, alloc::Allocator& allocator )
{
	RF_ASSERT( mWindow == nullptr );
	mWindow = &window;
	mIdentifier = identifier;
	WeakPtr<rollback::Stream<Type>> const retVal = window.GetOrCreateStream<Type>( mIdentifier, allocator );
	RF_ASSERT( retVal != nullptr );
	mVar = retVal;
	return retVal;
}



template<typename T>
UniquePtr<rollback::Stream<typename AutoVar<T>::Type>> AutoVar<T>::Unbind()
{
	RF_ASSERT( mWindow != nullptr );
	UniquePtr<rollback::Stream<Type>> retVal = mWindow->RemoveStream<Type>( mIdentifier );
	RF_ASSERT( retVal != nullptr );
	mWindow = nullptr;
	mIdentifier.clear();
	mVar = Var<Type>{ nullptr };
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

static_assert( RF::state::StateBag<1>::ContainedTypes::kNumTypes == 9, "Unexpected size" );
template class ROLLBACK_API RF::rollback::AutoVar<bool>;
template class ROLLBACK_API RF::rollback::AutoVar<uint8_t>;
template class ROLLBACK_API RF::rollback::AutoVar<int8_t>;
template class ROLLBACK_API RF::rollback::AutoVar<uint16_t>;
template class ROLLBACK_API RF::rollback::AutoVar<int16_t>;
template class ROLLBACK_API RF::rollback::AutoVar<uint32_t>;
template class ROLLBACK_API RF::rollback::AutoVar<int32_t>;
template class ROLLBACK_API RF::rollback::AutoVar<uint64_t>;
template class ROLLBACK_API RF::rollback::AutoVar<int64_t>;
