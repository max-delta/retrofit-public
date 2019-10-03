#pragma once
#include "core_state/StateStream.h"

#include "core_allocate/Allocator.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/unordered_map"
#include "rftl/shared_mutex"
#include "rftl/string"


namespace RF { namespace state {
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

	// TODO: This should be a class instead that has scope knowledge
	using VariableIdentifier = rftl::string;

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

	WeakPtr<StreamType const> GetStream( VariableIdentifier const& identifier ) const;
	WeakPtr<StreamType> GetMutableStream( VariableIdentifier const& identifier );


	//
	// Private data
private:
	mutable ReaderWriterMutex mMultiReaderSingleWriterLock;
	Tree mTree;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "StateTree.inl"
