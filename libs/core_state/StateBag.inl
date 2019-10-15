#pragma once
#include "core_state/StateBag.h"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxChangesT>
template<typename T>
inline WeakPtr<typename StateBag<MaxChangesT>::template Stream<T>> StateBag<MaxChangesT>::GetOrCreateStream( VariableIdentifier const& identifier, alloc::Allocator& allocator )
{
	return GetMutableTree<T>().GetOrCreateStream( identifier, allocator );
}



template<size_t MaxChangesT>
template<typename T>
inline UniquePtr<typename StateBag<MaxChangesT>::template Stream<T>> StateBag<MaxChangesT>::RemoveStream( VariableIdentifier const& identifier )
{
	return GetMutableTree<T>().RemoveStream( identifier );
}



template<size_t MaxChangesT>
inline size_t StateBag<MaxChangesT>::RemoveAllStreams()
{
	size_t retVal = 0;
	static_assert( ContainedTypes::kNumTypes == 8, "Unexpected size" );
	retVal += mU8.RemoveAllStreams();
	retVal += mS8.RemoveAllStreams();
	retVal += mU16.RemoveAllStreams();
	retVal += mS16.RemoveAllStreams();
	retVal += mU32.RemoveAllStreams();
	retVal += mS32.RemoveAllStreams();
	retVal += mU64.RemoveAllStreams();
	retVal += mS64.RemoveAllStreams();
	return retVal;
}



template<size_t MaxChangesT>
template<typename T>
inline WeakPtr<typename StateBag<MaxChangesT>::template Stream<T> const> StateBag<MaxChangesT>::GetStream( VariableIdentifier const& identifier ) const
{
	return GetTree<T>().GetStream( identifier );
}



template<size_t MaxChangesT>
template<typename T>
inline WeakPtr<typename StateBag<MaxChangesT>::template Stream<T>> StateBag<MaxChangesT>::GetMutableStream( VariableIdentifier const& identifier )
{
	return GetMutableTree<T>().GetMutableStream( identifier );
}



template<size_t MaxChangesT>
template<typename T>
inline typename StateBag<MaxChangesT>::template Tree<T> const& RF::state::StateBag<MaxChangesT>::GetTree() const
{
	// Sadly, a weakness in the language means we can't use specialization here
	static_assert( ContainedTypes::kNumTypes == 8, "Unexpected size" );
	if constexpr( false )
	{
		//
	}
#define RF_STATE_TREE( TYPE, VAR ) \
	else if constexpr( rftl::is_same<T, TYPE>::value ) \
	{ \
		return VAR; \
	}
	RF_STATE_TREE( uint8_t, mU8 )
	RF_STATE_TREE( int8_t, mS8 )
	RF_STATE_TREE( uint16_t, mU16 )
	RF_STATE_TREE( int16_t, mS16 )
	RF_STATE_TREE( uint32_t, mU32 )
	RF_STATE_TREE( int32_t, mS32 )
	RF_STATE_TREE( uint64_t, mU64 )
	RF_STATE_TREE( int64_t, mS64 )
#undef RF_STATE_TREE
	else
	{
		RF_RETAIL_FATAL_MSG( "GetTree", "Bad constexpr code gen" );
	}
}



template<size_t MaxChangesT>
template<typename T>
inline typename StateBag<MaxChangesT>::template Tree<T>& RF::state::StateBag<MaxChangesT>::GetMutableTree()
{
	// Sadly, a weakness in the language means we can't use specialization here
	static_assert( ContainedTypes::kNumTypes == 8, "Unexpected size" );
	if constexpr( false )
	{
		//
	}
#define RF_STATE_TREE( TYPE, VAR ) \
	else if constexpr( rftl::is_same<T, TYPE>::value ) \
	{ \
		return VAR; \
	}
	RF_STATE_TREE( uint8_t, mU8 )
	RF_STATE_TREE( int8_t, mS8 )
	RF_STATE_TREE( uint16_t, mU16 )
	RF_STATE_TREE( int16_t, mS16 )
	RF_STATE_TREE( uint32_t, mU32 )
	RF_STATE_TREE( int32_t, mS32 )
	RF_STATE_TREE( uint64_t, mU64 )
	RF_STATE_TREE( int64_t, mS64 )
#undef RF_STATE_TREE
	else
	{
		RF_RETAIL_FATAL_MSG( "GetTree", "Bad constexpr code gen" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
