#include "stdafx.h"
#include "Var.h"

#include "Timing/FrameClock.h"

#include "core_state/StateStream.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
Var<T>::Var( WeakPtr<rollback::Stream<Type>> const& var )
	: mBackingVar( var )
{
	//
}



template<typename T>
Var<T>::Var( WeakPtr<rollback::Stream<Type>>&& var )
	: mBackingVar( rftl::move( var ) )
{
	//
}



template<typename T>
Var<T>& Var<T>::operator=( WeakPtr<rollback::Stream<Type>> const& var )
{
	mBackingVar = var;
	return *this;
}



template<typename T>
Var<T>& Var<T>::operator=( WeakPtr<rollback::Stream<Type>>&& var )
{
	mBackingVar = rftl::move( var );
	return *this;
}



template<typename T>
Var<T>& Var<T>::operator=( Type const& value )
{
	Write( time::FrameClock::now(), value );
	return *this;
}



template<typename T>
Var<T>::operator Type() const
{
	return As();
}



template<typename T>
void Var<T>::Write( time::CommonClock::time_point time, Type const& value )
{
	RF_ASSERT( mBackingVar != nullptr );
	mBackingVar->Write( time, value );
}



template<typename T>
typename Var<T>::Type Var<T>::Read( time::CommonClock::time_point time ) const
{
	RF_ASSERT( mBackingVar != nullptr );
	return mBackingVar->Read( time );
}



template<typename T>
typename Var<T>::Type Var<T>::As() const
{
	return Read( time::FrameClock::now() );
}

///////////////////////////////////////////////////////////////////////////////
}}

template class ROLLBACK_API RF::rollback::Var<uint8_t>;
template class ROLLBACK_API RF::rollback::Var<int8_t>;
template class ROLLBACK_API RF::rollback::Var<uint16_t>;
template class ROLLBACK_API RF::rollback::Var<int16_t>;
template class ROLLBACK_API RF::rollback::Var<uint32_t>;
template class ROLLBACK_API RF::rollback::Var<int32_t>;
template class ROLLBACK_API RF::rollback::Var<uint64_t>;
template class ROLLBACK_API RF::rollback::Var<int64_t>;
