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
	static_assert( Snapshot::second_type::ContainedTypes::kNumTypes == 9, "Unexpected size" );
	details::WriteAllIdentifiers<bool>( dest.second.GetMutableTree<bool>(), src.GetTree<bool>(), time, allocator );
	details::WriteAllIdentifiers<uint8_t>( dest.second.GetMutableTree<uint8_t>(), src.GetTree<uint8_t>(), time, allocator );
	details::WriteAllIdentifiers<int8_t>( dest.second.GetMutableTree<int8_t>(), src.GetTree<int8_t>(), time, allocator );
	details::WriteAllIdentifiers<uint16_t>( dest.second.GetMutableTree<uint16_t>(), src.GetTree<uint16_t>(), time, allocator );
	details::WriteAllIdentifiers<int16_t>( dest.second.GetMutableTree<int16_t>(), src.GetTree<int16_t>(), time, allocator );
	details::WriteAllIdentifiers<uint32_t>( dest.second.GetMutableTree<uint32_t>(), src.GetTree<uint32_t>(), time, allocator );
	details::WriteAllIdentifiers<int32_t>( dest.second.GetMutableTree<int32_t>(), src.GetTree<int32_t>(), time, allocator );
	details::WriteAllIdentifiers<uint64_t>( dest.second.GetMutableTree<uint64_t>(), src.GetTree<uint64_t>(), time, allocator );
	details::WriteAllIdentifiers<int64_t>( dest.second.GetMutableTree<int64_t>(), src.GetTree<int64_t>(), time, allocator );
}



void ReadFromSnapshot( Window& dest, Snapshot const& src, alloc::Allocator& allocator )
{
	time::CommonClock::time_point const time = src.first;

	// TODO: Unpack typelist instead of copy-paste
	static_assert( Snapshot::second_type::ContainedTypes::kNumTypes == 9, "Unexpected size" );
	details::WriteAllIdentifiers<bool>( dest.GetMutableTree<bool>(), src.second.GetTree<bool>(), time, allocator );
	details::WriteAllIdentifiers<uint8_t>( dest.GetMutableTree<uint8_t>(), src.second.GetTree<uint8_t>(), time, allocator );
	details::WriteAllIdentifiers<int8_t>( dest.GetMutableTree<int8_t>(), src.second.GetTree<int8_t>(), time, allocator );
	details::WriteAllIdentifiers<uint16_t>( dest.GetMutableTree<uint16_t>(), src.second.GetTree<uint16_t>(), time, allocator );
	details::WriteAllIdentifiers<int16_t>( dest.GetMutableTree<int16_t>(), src.second.GetTree<int16_t>(), time, allocator );
	details::WriteAllIdentifiers<uint32_t>( dest.GetMutableTree<uint32_t>(), src.second.GetTree<uint32_t>(), time, allocator );
	details::WriteAllIdentifiers<int32_t>( dest.GetMutableTree<int32_t>(), src.second.GetTree<int32_t>(), time, allocator );
	details::WriteAllIdentifiers<uint64_t>( dest.GetMutableTree<uint64_t>(), src.second.GetTree<uint64_t>(), time, allocator );
	details::WriteAllIdentifiers<int64_t>( dest.GetMutableTree<int64_t>(), src.second.GetTree<int64_t>(), time, allocator );
}

///////////////////////////////////////////////////////////////////////////////
}}
