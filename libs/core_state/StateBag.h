#pragma once
#include "core_state/StateTree.h"

#include "core/meta/TypeList.h"


namespace RF::state {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxChangesT>
class StateBag
{
	RF_NO_COPY( StateBag );
	RF_NO_MOVE( StateBag );

	//
	// Types and constants
public:
	static constexpr size_t kMaxChanges = MaxChangesT;

	using ContainedTypes = TypeList<
		bool,
		uint8_t, int8_t,
		uint16_t, int16_t,
		uint32_t, int32_t,
		uint64_t, int64_t>;

	template<typename T>
	using Tree = StateTree<T, kMaxChanges>;
	template<typename T>
	using Stream = typename Tree<T>::StreamType;

private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Public methods
public:
	StateBag() = default;
	~StateBag() = default;

	template<typename T>
	WeakPtr<Stream<T>> GetOrCreateStream( VariableIdentifier const& identifier, alloc::Allocator& allocator );

	template<typename T>
	UniquePtr<Stream<T>> RemoveStream( VariableIdentifier const& identifier );

	size_t RemoveAllStreams();

	template<typename T>
	WeakPtr<Stream<T> const> GetStream( VariableIdentifier const& identifier ) const;

	template<typename T>
	WeakPtr<Stream<T>> GetMutableStream( VariableIdentifier const& identifier );

	size_t RewindAllStreams( time::CommonClock::time_point time );

	template<typename T>
	Tree<T> const& GetTree() const;

	template<typename T>
	Tree<T>& GetMutableTree();


	//
	// Private data
private:
	Tree<bool> mB;
	Tree<uint8_t> mU8;
	Tree<int8_t> mS8;
	Tree<uint16_t> mU16;
	Tree<int16_t> mS16;
	Tree<uint32_t> mU32;
	Tree<int32_t> mS32;
	Tree<uint64_t> mU64;
	Tree<int64_t> mS64;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "StateBag.inl"
