#pragma once
#include "core_state/StateStream.h"
#include "core_state/VariableIdentifier.h"

#include "core_allocate/Allocator.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/unordered_map"
#include "rftl/vector"
#include "rftl/shared_mutex"
#include "rftl/string"


namespace RF::state {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueT, size_t MaxChangesT>
class StateTree
{
	RF_NO_COPY( StateTree );
	RF_NO_MOVE( StateTree );

	//
	// Types and constants
public:
	using ValueType = ValueT;
	static constexpr size_t kMaxChanges = MaxChangesT;
	using StreamType = StateStream<ValueType, kMaxChanges>;

private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;
	using Tree = rftl::unordered_map<VariableIdentifier, UniquePtr<StreamType>>;


	//
	// Public methods
public:
	StateTree() = default;
	~StateTree() = default;

	WeakPtr<StreamType> GetOrCreateStream( VariableIdentifier const& identifier, alloc::Allocator& allocator );
	UniquePtr<StreamType> RemoveStream( VariableIdentifier const& identifier );
	size_t RemoveAllStreams();

	WeakPtr<StreamType const> GetStream( VariableIdentifier const& identifier ) const;
	WeakPtr<StreamType> GetMutableStream( VariableIdentifier const& identifier );

	size_t RewindAllStreams( time::CommonClock::time_point time );

	rftl::vector<VariableIdentifier> GetIdentifiers() const;


	//
	// Private data
private:
	mutable ReaderWriterMutex mMultiReaderSingleWriterLock;
	Tree mTree;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "StateTree.inl"
