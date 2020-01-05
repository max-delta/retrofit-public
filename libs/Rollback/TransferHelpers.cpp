#include "stdafx.h"
#include "TransferHelpers.h"

#include "core_state/StateBag.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename ValueT, size_t MaxChangesT, size_t MaxChangesU>
void WriteAllIdentifiers( state::StateTree<ValueT, MaxChangesT>& dest, state::StateTree<ValueT, MaxChangesU> const& src, time::CommonClock::time_point time, alloc::Allocator& allocator )
{
	using namespace state;

	rftl::vector<VariableIdentifier> const identifiers = src.GetIdentifiers();
	for( VariableIdentifier const& identifier : identifiers )
	{
		WeakPtr<StateStream<ValueT, MaxChangesU> const> const sourceStream = src.GetStream( identifier );
		RF_ASSERT( sourceStream != nullptr );
		ValueT const value = sourceStream->Read( time );

		WeakPtr<StateStream<ValueT, MaxChangesT>> const destStream = dest.GetOrCreateStream( identifier, allocator );
		RF_ASSERT( destStream != nullptr );
		destStream->Write( time, value );
	}
}

}
///////////////////////////////////////////////////////////////////////////////

void WriteToSnapshot( Snapshot& dest, Window const& src, time::CommonClock::time_point time, alloc::Allocator& allocator )
{
	// TODO: Unpack typelist instead of copy-paste
	using ContainedTypes = Snapshot::second_type::ContainedTypes;
	static_assert( ContainedTypes::kNumTypes == 9, "Unexpected size" );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<0>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<0>::type>(), src.GetTree<ContainedTypes::ByIndex<0>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<1>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<1>::type>(), src.GetTree<ContainedTypes::ByIndex<1>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<2>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<2>::type>(), src.GetTree<ContainedTypes::ByIndex<2>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<3>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<3>::type>(), src.GetTree<ContainedTypes::ByIndex<3>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<4>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<4>::type>(), src.GetTree<ContainedTypes::ByIndex<4>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<5>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<5>::type>(), src.GetTree<ContainedTypes::ByIndex<5>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<6>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<6>::type>(), src.GetTree<ContainedTypes::ByIndex<6>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<7>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<7>::type>(), src.GetTree<ContainedTypes::ByIndex<7>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<8>::type>( dest.second.GetMutableTree<ContainedTypes::ByIndex<8>::type>(), src.GetTree<ContainedTypes::ByIndex<8>::type>(), time, allocator );
}



void ReadFromSnapshot( Window& dest, Snapshot const& src, alloc::Allocator& allocator )
{
	time::CommonClock::time_point const time = src.first;

	// TODO: Unpack typelist instead of copy-paste
	using ContainedTypes = Snapshot::second_type::ContainedTypes;
	static_assert( ContainedTypes::kNumTypes == 9, "Unexpected size" );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<0>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<0>::type>(), src.second.GetTree<ContainedTypes::ByIndex<0>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<1>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<1>::type>(), src.second.GetTree<ContainedTypes::ByIndex<1>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<2>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<2>::type>(), src.second.GetTree<ContainedTypes::ByIndex<2>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<3>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<3>::type>(), src.second.GetTree<ContainedTypes::ByIndex<3>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<4>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<4>::type>(), src.second.GetTree<ContainedTypes::ByIndex<4>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<5>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<5>::type>(), src.second.GetTree<ContainedTypes::ByIndex<5>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<6>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<6>::type>(), src.second.GetTree<ContainedTypes::ByIndex<6>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<7>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<7>::type>(), src.second.GetTree<ContainedTypes::ByIndex<7>::type>(), time, allocator );
	details::WriteAllIdentifiers<ContainedTypes::ByIndex<8>::type>( dest.GetMutableTree<ContainedTypes::ByIndex<8>::type>(), src.second.GetTree<ContainedTypes::ByIndex<8>::type>(), time, allocator );
}

///////////////////////////////////////////////////////////////////////////////
}}
