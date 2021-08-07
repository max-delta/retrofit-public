#pragma once
#include "core_state/StateBag.h"


namespace RF::state {
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
	static_assert( ContainedTypes::kNumTypes == 9, "Unexpected size" );
	retVal += GetMutableTree<ContainedTypes::ByIndex<0>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<1>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<2>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<3>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<4>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<5>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<6>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<7>::type>().RemoveAllStreams();
	retVal += GetMutableTree<ContainedTypes::ByIndex<8>::type>().RemoveAllStreams();
	return retVal;
}



template<size_t MaxChangesT>
inline size_t StateBag<MaxChangesT>::RewindAllStreams( time::CommonClock::time_point time )
{
	size_t retVal = 0;
	static_assert( ContainedTypes::kNumTypes == 9, "Unexpected size" );
	retVal += GetMutableTree<ContainedTypes::ByIndex<0>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<1>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<2>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<3>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<4>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<5>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<6>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<7>::type>().RewindAllStreams( time );
	retVal += GetMutableTree<ContainedTypes::ByIndex<8>::type>().RewindAllStreams( time );
	return retVal;
}



template<size_t MaxChangesT>
inline rftl::optional<InclusiveTimeRange> StateBag<MaxChangesT>::GetEarliestTimes() const
{
	rftl::optional<InclusiveTimeRange> retVal;
	auto const expand = [&retVal]( rftl::optional<InclusiveTimeRange> range ) -> void //
	{
		if( range.has_value() == false )
		{
			return;
		}
		if( retVal.has_value() == false )
		{
			retVal = range;
			return;
		}
		retVal->first = math::Min( retVal->first, range->first );
		retVal->second = math::Max( retVal->second, range->second );
	};
	auto const expandTree = [&expand]( auto const& tree ) -> void //
	{
		expand( tree.GetEarliestTimes() );
	};
	expandTree( GetTree<ContainedTypes::ByIndex<0>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<1>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<2>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<3>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<4>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<5>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<6>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<7>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<8>::type>() );
	return retVal;
}



template<size_t MaxChangesT>
inline rftl::optional<InclusiveTimeRange> StateBag<MaxChangesT>::GetLatestTimes() const
{
	rftl::optional<InclusiveTimeRange> retVal;
	auto const expand = [&retVal]( rftl::optional<InclusiveTimeRange> range ) -> void //
	{
		if( range.has_value() == false )
		{
			return;
		}
		if( retVal.has_value() == false )
		{
			retVal = range;
			return;
		}
		retVal->first = math::Min( retVal->first, range->first );
		retVal->second = math::Max( retVal->second, range->second );
	};
	auto const expandTree = [&expand]( auto const& tree ) -> void //
	{
		expand( tree.GetLatestTimes() );
	};
	expandTree( GetTree<ContainedTypes::ByIndex<0>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<1>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<2>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<3>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<4>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<5>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<6>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<7>::type>() );
	expandTree( GetTree<ContainedTypes::ByIndex<8>::type>() );
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
	static_assert( ContainedTypes::kNumTypes == 9, "Unexpected size" );
	if constexpr( false )
	{
		//
	}
#define RF_STATE_TREE( TYPE, VAR ) \
	else if constexpr( rftl::is_same<T, TYPE>::value ) \
	{ \
		return VAR; \
	}
	RF_STATE_TREE( bool, mB )
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
	static_assert( ContainedTypes::kNumTypes == 9, "Unexpected size" );
	if constexpr( false )
	{
		//
	}
#define RF_STATE_TREE( TYPE, VAR ) \
	else if constexpr( rftl::is_same<T, TYPE>::value ) \
	{ \
		return VAR; \
	}
	RF_STATE_TREE( bool, mB )
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
}
