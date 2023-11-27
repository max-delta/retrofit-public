#include "stdafx.h"
#include "ObjectDeserializer.h"

#include "Logging/Logging.h"
#include "Serialization/Importer.h"
#include "Serialization/ObjectInstance.h"

#include "core_math/math_clamps.h"
#include "core_rftype/ConstructedType.h"
#include "core_rftype/TypeTraverser.h"

#include "core/meta/ScopedCleanup.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

RF_TODO_ANNOTATION( "Remove c_str usages, update logging to C++20 format" );
#include "rftl/extension/c_str.h"

#include "rftl/extension/string_format.h"
#include "rftl/optional"
#include "rftl/unordered_map"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr char kThis[] = "!this";
static constexpr char kUnset[] = "";



struct PendingAccessorKeyData
{
	// The type of the key
	reflect::VariableTypeInfo mKeyTypeInfo = {};

	// Values don't need fancy construction, so can exist as just bytes
	rftl::vector<uint8_t> mValueStorage = {};

	RF_TODO_ANNOTATION( "Support for keys that aren't value types" );
};



struct PendingAccessorTargetData
{
	// The type of the target
	reflect::VariableTypeInfo mTargetTypeInfo = {};

	// TODO: This applies to things that don't support default construction,
	//  and things that don't allow mutable access to their contents
	RF_TODO_ANNOTATION( "Support for accessors that need targets to be created externally" );
};



struct UnlinkedAccessorIndirectionData
{
	// The indirection's target
	// NOTE: One, and ONLY one, of these must be set
	rftl::optional<exporter::InstanceID> mLocalInstanceID = rftl::nullopt;
	rftl::optional<rftl::string> mExternalReferenceID = rftl::nullopt;
};



struct WalkNode
{
	RF_NO_COPY( WalkNode );

	WalkNode() = default;

	WalkNode( rftl::string&& identifier )
		: mIdentifier( rftl::move( identifier ) )
	{
		//
	}

	~WalkNode()
	{
		RF_ASSERT( mAccessorOpened == false );
	}

	rftl::string mIdentifier;

	rftl::optional<reflect::VariableTypeInfo const> mVariableTypeInfo = rftl::nullopt;
	void* mVariableLocation = nullptr;

	// On an accessor node, whether the accessor has been opened for mutation
	//  operations or not, which must be done before accessing / modifying and
	//  must be closed when done
	bool mAccessorOpened = false;

	// Pending key stores the backing data for a key while it's still being
	//  composed, and recent key stores the root node for that key after it has
	//  closed and has been inserted in the container
	// NOTE: Pending key must persist while the recent key is present, as it
	//  stores the backing key data the target node needs to reference in order
	//  to find the storage location for the target in the accessor
	rftl::optional<PendingAccessorKeyData> mPendingKey;
	UniquePtr<WalkNode> mRecentKey = nullptr;

	// Pending target stores the backing data for a target while it's still
	//  being composed
	rftl::optional<PendingAccessorTargetData> mPendingTarget;

	// Stores information about an indirection that is needed to properly
	//  resolve the node
	rftl::optional<UnlinkedAccessorIndirectionData> mUnlinkedIndirectionData;

	// If on an accessor, these are used to help determine what what index
	//  we're on
	size_t numKeyStartsObserved = 0;
	size_t numTargetStartsObserved = 0;
};

using WalkChain = rftl::vector<UniquePtr<WalkNode>>;



struct DeferredInstance
{
	exporter::InstanceID mInstanceID = exporter::kInvalidInstanceID;
	rftl::optional<exporter::TypeID> mTypeID = rftl::nullopt;
};



struct ScratchObjectStorage
{
	// The preferred storage, where IDs are in use
	using ObjectInstancesByID = rftl::unordered_map<exporter::InstanceID, UniquePtr<ObjectInstance>>;
	ObjectInstancesByID mObjectInstancesByID;

	// Non-ideal, not all objects will have instance IDs, as they are optional
	// NOTE: This is not strictly an issue, but means that indirections can't
	//  be performed
	// NOTE: Since there's no ID for them, they just migrate in-order from
	//  a 'fresh' queue to a 'filled out' queue as they get deserialized into
	using ObjectInstancesWithoutIDs = rftl::deque<UniquePtr<ObjectInstance>>;
	ObjectInstancesWithoutIDs mObjectInstancesWithoutIDs_Fresh;
	ObjectInstancesWithoutIDs mObjectInstancesWithoutIDs_FilledOut;
};



struct ScratchSpace
{
	RF_NO_COPY( ScratchSpace );

	ScratchSpace(
		ObjectDeserializer::Params const& params )
		: mParams( params )
	{
		//
	}

	ObjectDeserializer::Params const& mParams;

	size_t mInstanceCount = 0;
	WalkChain mWalkChain;

	rftl::optional<DeferredInstance> mDeferredInstance = rftl::nullopt;

	using TOCEntries = rftl::unordered_map<exporter::InstanceID, rftl::optional<exporter::TypeID>>;
	TOCEntries mTOCEntries;

	using LocalIndirections = rftl::unordered_map<exporter::IndirectionID, exporter::InstanceID>;
	LocalIndirections mLocalIndirections;

	using ExternalIndirections = rftl::unordered_map<exporter::IndirectionID, rftl::string>;
	ExternalIndirections mExternalIndirections;

	ScratchObjectStorage mScratchObjectStorage = {};

	// HACK: Support the one-time non-constructing deserialization root case
	RF_TODO_ANNOTATION( "Re-evaluate how to do this" );
	WeakPtr<ObjectInstance> mOneTimeSingleRootTOCOverride_Ref = nullptr;
};



void EnsureAccessorOpened( WalkNode& accessorNode )
{
	RF_ASSERT( accessorNode.mVariableTypeInfo->mAccessor != nullptr );

	// Is it closed?
	if( accessorNode.mAccessorOpened == false )
	{
		reflect::ExtensionAccessor const& accessor = *accessorNode.mVariableTypeInfo->mAccessor;
		void* const accessorLocation = accessorNode.mVariableLocation;
		RF_ASSERT( accessorLocation != nullptr );

		// Open it
		if( accessor.mBeginMutation != nullptr )
		{
			accessor.mBeginMutation( accessorLocation );
		}
		accessorNode.mAccessorOpened = true;
	}
}



void EnsureAccessorClosed( WalkNode& accessorNode )
{
	RF_ASSERT( accessorNode.mVariableTypeInfo->mAccessor != nullptr );

	// Is it open?
	if( accessorNode.mAccessorOpened )
	{
		reflect::ExtensionAccessor const& accessor = *accessorNode.mVariableTypeInfo->mAccessor;
		void* const accessorLocation = accessorNode.mVariableLocation;
		RF_ASSERT( accessorLocation != nullptr );

		// Close it
		if( accessor.mEndMutation != nullptr )
		{
			accessor.mEndMutation( accessorLocation );
		}
		accessorNode.mAccessorOpened = false;
	}
}



bool IsWalkNodeResolved( WalkNode const& walkNode )
{
	RF_TODO_ANNOTATION(
		"What about accessor keys and targets?"
		" Do they really require identifiers?" );

	RF_ASSERT( walkNode.mIdentifier.empty() == false );

	if( walkNode.mVariableLocation == nullptr )
	{
		RF_ASSERT( walkNode.mVariableTypeInfo.has_value() == false );
		return false;
	}

	RF_ASSERT( walkNode.mVariableTypeInfo.has_value() );
	RF_ASSERT( walkNode.mVariableLocation != nullptr );
	return true;
}



void DestroyChain( WalkChain& fullChain )
{
	while( fullChain.empty() == false )
	{
		// Pop
		UniquePtr<WalkNode> const extracted = rftl::move( fullChain.back() );
		fullChain.pop_back();

		if( extracted == nullptr )
		{
			// Placeholder node
			continue;
		}
		WalkNode& node = *extracted;

		if( node.mVariableTypeInfo.has_value() == false )
		{
			// Typeless node
			continue;
		}
		RF_ASSERT( node.mVariableLocation != nullptr );
		reflect::VariableTypeInfo const& typeInfo = *node.mVariableTypeInfo;

		if( typeInfo.mAccessor != nullptr )
		{
			// Popped an accessor
			WalkNode& accessorNode = *extracted;
			EnsureAccessorClosed( accessorNode );
			continue;
		}
	}
}



bool PopFromChain( WalkChain& fullChain )
{
	RF_ASSERT( fullChain.empty() == false );

	UniquePtr<WalkNode> extracted = rftl::move( fullChain.back() );
	RF_ASSERT( extracted != nullptr );
	RF_ASSERT( extracted->mVariableTypeInfo.has_value() );
	fullChain.pop_back();

	RF_ASSERT( fullChain.back() != nullptr );
	WalkNode& current = *fullChain.back();

	RF_TODO_ANNOTATION(
		"What about accessor keys and targets?"
		" Do they really require identifiers?" );
	RF_ASSERT( current.mIdentifier.empty() == false );
	RF_ASSERT( current.mVariableTypeInfo.has_value() );
	RF_ASSERT( current.mVariableLocation != nullptr );

	// Popped an accessor
	if( extracted->mVariableTypeInfo->mAccessor != nullptr )
	{
		WalkNode& accessorNode = *extracted;
		EnsureAccessorClosed( accessorNode );
	}

	// Child of a value type
	if( current.mVariableTypeInfo->mValueType != reflect::Value::Type::Invalid )
	{
		RF_TODO_BREAK_MSG( "How did a child of a value type end up on the chain?" );
		return true;
	}

	// Member on a class
	if( current.mVariableTypeInfo->mClassInfo != nullptr )
	{
		return true;
	}

	// Node on an extension accessor
	if( current.mVariableTypeInfo->mAccessor != nullptr )
	{
		WalkNode const& accessorNode = current;
		reflect::ExtensionAccessor const& accessor = *accessorNode.mVariableTypeInfo->mAccessor;
		void const* const accessorLocation = accessorNode.mVariableLocation;
		RF_ASSERT( accessorLocation != nullptr );
		rftl::string const& nodeIdentifier = extracted->mIdentifier;

		if( nodeIdentifier == "K" )
		{
			// Key

			WalkNode const& keyNode = *extracted;

			// Store key node in the accessor node, and await for the
			//  corresponding target node to arrive
			RF_ASSERT( current.mPendingKey.has_value() );
			RF_ASSERT( current.mPendingKey->mValueStorage.data() == keyNode.mVariableLocation );
			RF_ASSERT( current.mPendingKey->mKeyTypeInfo == keyNode.mVariableTypeInfo );
			RF_ASSERT( current.mRecentKey == nullptr );
			current.mRecentKey = rftl::move( extracted );

			return true;
		}

		if( nodeIdentifier == "T" )
		{
			// Target

			WalkNode const& targetNode = *extracted;

			RF_TODO_ANNOTATION(
				"Currently assuming that no delayed accessor operations are"
				" needed, which might not be true when more restrictive"
				" accessors need to be supported" );
			( (void)accessor );

			// Wipe out the key and target data
			RF_ASSERT( current.mPendingKey.has_value() );
			RF_ASSERT( current.mRecentKey != nullptr );
			RF_ASSERT( current.mRecentKey->mVariableTypeInfo == current.mPendingKey->mKeyTypeInfo );
			RF_ASSERT( current.mPendingTarget.has_value() );
			RF_ASSERT( targetNode.mVariableTypeInfo == current.mPendingTarget->mTargetTypeInfo );
			current.mPendingTarget.reset();
			current.mRecentKey = nullptr;
			current.mPendingKey.reset();

			return true;
		}

		RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Unknown extension node identifier '%s'", nodeIdentifier.c_str() );
		RF_DBGFAIL();
		return false;
	}

	RF_TODO_ANNOTATION( "At time of writing, the implementation of complex types is undecided, and will affect this code possibility" );
	RF_TODO_BREAK_MSG( "Can maybe get here if a complex key type was a parent? Depends on how that's implemented?" );
	RF_TODO_BREAK_MSG( "Can maybe get here if a complex target type was a parent? Depends on how that's implemented?" );

	RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Unexpected codepath, can't determine variable type of predecessors" );
	RF_DBGFAIL();
	return false;
}



bool ReplaceEndOfChain( WalkChain& fullChain, UniquePtr<WalkNode>&& newNode )
{
	RF_ASSERT( fullChain.empty() == false );
	RF_ASSERT( newNode != nullptr );

	if( fullChain.back() == nullptr )
	{
		// Null node, presumed to be a fresh indentation
		fullChain.pop_back();
	}
	else if( IsWalkNodeResolved( *fullChain.back() ) == false )
	{
		// Unresolved node, dubious
		RFLOG_WARNING( fullChain, RFCAT_SERIALIZATION,
			"Replacing an unresolved end-of-chain node, this might imply that"
			" a property was mentioned by the importer, but had no meaningful"
			" actions associated with it, which would likely indicate wasteful"
			" serialization" );
		RF_TODO_ANNOTATION( "Fix any cases of wasteful exporter serialization" );
		fullChain.pop_back();
	}
	else
	{
		// Valid node, make sure to run proper pop logic
		bool const popSuccess = PopFromChain( fullChain );
		if( popSuccess == false )
		{
			return false;
		}
	}

	fullChain.emplace_back( rftl::move( newNode ) );

	return true;
}



bool SetValueToChain( WalkChain& fullChain, reflect::Value const& incomingValue )
{
	RF_ASSERT( fullChain.empty() == false );
	RF_ASSERT( incomingValue.GetStoredType() != reflect::Value::Type::Invalid );

	RF_ASSERT( fullChain.back() != nullptr );
	WalkNode& current = *fullChain.back();

	RF_TODO_ANNOTATION(
		"What about accessor keys and targets?"
		" Do they really require identifiers?" );
	RF_ASSERT( current.mIdentifier.empty() == false );
	RF_ASSERT( current.mVariableTypeInfo.has_value() );
	RF_ASSERT( current.mVariableLocation != nullptr );

	// Value type
	if( current.mVariableTypeInfo->mValueType != reflect::Value::Type::Invalid )
	{
		reflect::Value::Type const& destType = current.mVariableTypeInfo->mValueType;
		reflect::Value::Type const srcType = incomingValue.GetStoredType();

		void* const& destBytes = current.mVariableLocation;
		size_t const numBytesToWrite = reflect::Value::GetNumBytesNeeded( destType );

		if( srcType != destType )
		{
			RF_TODO_BREAK_MSG( "Support for converting incoming values" );
			return false;
		}

		RF_ASSERT( srcType == destType );
		rftl::byte_view const srcBytes = incomingValue.GetBytes();
		srcBytes.mem_copy_to( destBytes, numBytesToWrite );

		return true;
	}

	// Nested class
	if( current.mVariableTypeInfo->mClassInfo != nullptr )
	{
		RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Attempting value set on a class instance" );
		RF_DBGFAIL();
		return false;
	}

	// Extension accessor
	if( current.mVariableTypeInfo->mAccessor != nullptr )
	{
		RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Attempting value set on an accessor instance" );
		RF_DBGFAIL();
		return false;
	}

	RF_TODO_ANNOTATION( "At time of writing, the implementation of complex types is undecided, and will affect this code possibility" );
	RF_TODO_BREAK_MSG( "Can maybe get here if in a complex key type? Depends on how that's implemented?" );
	RF_TODO_BREAK_MSG( "Can maybe get here if in a complex target type? Depends on how that's implemented?" );

	RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Unexpected codepath, can't determine variable type" );
	RF_DBGFAIL();
	return false;
}



bool IsWalkChainLeadingEdgeResolved( WalkChain const& fullChain )
{
	if( fullChain.size() < 2 )
	{
		RF_DBGFAIL_MSG(
			"Walk chain is too small to be resolved, but asking if it is"
			" resolved, seems like a bug?" );
		return false;
	}

	RF_ASSERT( fullChain.back() != nullptr );
	WalkNode const& current = *fullChain.back();

	return IsWalkNodeResolved( current );
}



bool ResolveWalkChainLeadingEdge( WalkChain& fullChain, ScratchObjectStorage& scratchObjectStorage )
{
	if( fullChain.size() < 2 )
	{
		RF_DBGFAIL_MSG( "Can't resolve a chain without multiple nodes" );
		return false;
	}

	RF_ASSERT( fullChain.front() != nullptr );
	WalkNode const& first = *fullChain.front();
	RF_ASSERT( *( fullChain.rbegin() + 1 ) != nullptr );
	WalkNode& previous = **( fullChain.rbegin() + 1 );
	RF_ASSERT( fullChain.back() != nullptr );
	WalkNode& current = *fullChain.back();

	RF_TODO_ANNOTATION(
		"What about accessor keys and targets?"
		" Do they really require identifiers?" );
	RF_ASSERT( first.mIdentifier.empty() == false );
	RF_ASSERT( first.mVariableTypeInfo.has_value() );
	RF_ASSERT( first.mVariableLocation != nullptr );
	RF_ASSERT( previous.mIdentifier.empty() == false );
	RF_ASSERT( previous.mVariableTypeInfo.has_value() );
	RF_ASSERT( previous.mVariableLocation != nullptr );
	RF_ASSERT( current.mIdentifier.empty() == false );
	RF_ASSERT( current.mVariableTypeInfo.has_value() == false );
	RF_ASSERT( current.mVariableLocation == nullptr );
	current.mVariableTypeInfo.reset();
	current.mVariableLocation = nullptr;

	// Child of a value type?
	if( previous.mVariableTypeInfo->mValueType != reflect::Value::Type::Invalid )
	{
		// TODO: What about value keys in accessors?
		RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Attempting resolve on a value" );
		RF_DBGFAIL();
		return false;
	}

	// Member on a class
	if( previous.mVariableTypeInfo->mClassInfo != nullptr )
	{
		reflect::ClassInfo const& classToSearch = *previous.mVariableTypeInfo->mClassInfo;
		rftl::string const& memberNameToFind = current.mIdentifier;
		RF_ASSERT( memberNameToFind.empty() == false );

		// For each member...
		for( reflect::MemberVariableInfo const& varInfo : classToSearch.mNonStaticVariables )
		{
			if( varInfo.mIdentifier != memberNameToFind )
			{
				continue;
			}

			// Apply offset and note the type
			current.mVariableTypeInfo.emplace( varInfo.mVariableTypeInfo );
			current.mVariableLocation = reinterpret_cast<uint8_t*>( previous.mVariableLocation ) + varInfo.mOffset;
			return true;
		}

		RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Could not find member '%s'", memberNameToFind.c_str() );
		return false;
	}

	// Child of an accessor
	if( previous.mVariableTypeInfo->mAccessor != nullptr )
	{
		WalkNode& accessorNode = previous;
		reflect::ExtensionAccessor const& accessor = *accessorNode.mVariableTypeInfo->mAccessor;
		void* const accessorLocation = accessorNode.mVariableLocation;
		RF_ASSERT( accessorLocation != nullptr );
		rftl::string const& nodeIdentifier = current.mIdentifier;

		EnsureAccessorOpened( accessorNode );

		if( nodeIdentifier == "K" )
		{
			// Key

			// Wipe out old key information
			accessorNode.mRecentKey = nullptr;
			accessorNode.mPendingKey.reset();

			// Note the index, in case we need it
			size_t const theoreticalIndex = accessorNode.numKeyStartsObserved;
			accessorNode.numKeyStartsObserved++;

			RF_TODO_ANNOTATION( "Do we need the key index for anything?" );
			( (void)theoreticalIndex );

			if( accessor.mGetSharedKeyInfo == nullptr )
			{
				RF_TODO_BREAK_MSG( "Support for variantly-keyed accessors" );
				return false;
			}
			reflect::VariableTypeInfo const keyTypeInfo = accessor.mGetSharedKeyInfo( accessorLocation );

			if( keyTypeInfo.mValueType == reflect::Value::Type::Invalid )
			{
				RF_TODO_BREAK_MSG( "Support for complex-keyed accessors" );
				return false;
			}

			// Start new key
			PendingAccessorKeyData& pendingKey = accessorNode.mPendingKey.emplace();
			pendingKey.mKeyTypeInfo.mValueType = keyTypeInfo.mValueType;
			pendingKey.mValueStorage.resize( reflect::Value::GetNumBytesNeeded( keyTypeInfo.mValueType ) );

			RF_ASSERT( current.mVariableTypeInfo.has_value() == false );
			current.mVariableTypeInfo.emplace( pendingKey.mKeyTypeInfo );
			current.mVariableLocation = pendingKey.mValueStorage.data();

			return true;
		}

		if( nodeIdentifier == "T" )
		{
			// Target

			// Wipe out old target information
			accessorNode.mPendingTarget.reset();

			// Note the index, in case we need it
			size_t const theoreticalIndex = accessorNode.numTargetStartsObserved;
			accessorNode.numTargetStartsObserved++;

			if( accessorNode.mRecentKey == nullptr )
			{
				// No key, presumably this is a directly-keyed accessor
				RF_ASSERT( accessorNode.mPendingKey.has_value() == false );
				RF_ASSERT( accessorNode.numKeyStartsObserved == 0 );
				RF_TODO_BREAK_MSG( "Support for direct-keyed accessors" );
				( (void)theoreticalIndex );
				return false;
			}

			WalkNode const& keyNode = *accessorNode.mRecentKey;
			RF_ASSERT( keyNode.mVariableLocation != nullptr );
			RF_ASSERT( keyNode.mVariableTypeInfo.has_value() );
			void const* const& key = keyNode.mVariableLocation;
			reflect::VariableTypeInfo const& keyInfo = *keyNode.mVariableTypeInfo;

			if( accessor.mInsertVariableDefault != nullptr )
			{
				// Insert a placeholder

				bool const insertSuccess = accessor.mInsertVariableDefault( accessorLocation, key, keyInfo );
				if( insertSuccess == false )
				{
					RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Failed to insert key" );
					RF_DBGFAIL();
					return false;
				}
			}
			else if( accessor.mInsertVariableViaUPtr != nullptr )
			{
				// Insert a constructed placeholder

				if( current.mUnlinkedIndirectionData.has_value() )
				{
					// Fetch a pre-constructed instance and insert it

					RF_TODO_ANNOTATION(
						"This is all a little bit dangerous, since it expects"
						" that it can transfer memory into an accessor, and"
						" then continue to reference it, through potentially"
						" multiple insertions on the accessor, which is"
						" highly dubious. This is probably the strongest"
						" argument for deferring linking until the very end of"
						" deserialization, after all the instances have been"
						" filled out (minus their linkages)." );

					if( current.mUnlinkedIndirectionData->mLocalInstanceID.has_value() )
					{
						// Local instance

						RF_ASSERT( current.mUnlinkedIndirectionData->mExternalReferenceID.has_value() == false );

						exporter::InstanceID const& instanceID =
							current.mUnlinkedIndirectionData->mLocalInstanceID.value();

						// Find the instance by ID
						ScratchObjectStorage::ObjectInstancesByID::iterator const iter =
							scratchObjectStorage.mObjectInstancesByID.find( instanceID );
						if( iter == scratchObjectStorage.mObjectInstancesByID.end() )
						{
							RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION,
								"Could not locate a pre-created local instance"
								" to use to resolve an unlinked indirection" );
							RF_TODO_BREAK_MSG( "Support for deferred construction?" );
							return false;
						}
						UniquePtr<ObjectInstance>& handle = iter->second;
						RF_ASSERT( handle != nullptr );

						if( keyInfo.mValueType == reflect::Value::Type::Invalid )
						{
							RF_TODO_BREAK_MSG( "Support for complex-keyed accessors" );
							return false;
						}

						// Copy the key into a UniquePtr to transfer it
						// HACK: Assume it's a UInt64
						RF_TODO_ANNOTATION( "Proper cloning of value types" );
						reflect::Value const keyValue( keyInfo.mValueType, key );
						RF_ASSERT( keyInfo.mValueType == reflect::Value::Type::UInt64 );
						uint64_t const* const keyValuePtr = keyValue.GetAs<uint64_t>();
						RF_ASSERT( keyValuePtr != nullptr );
						UniquePtr<uint64_t> keyCopy = DefaultCreator<uint64_t>::Create( *keyValuePtr );

						// Prep the type info
						reflect::VariableTypeInfo transferTypeInfo = {};
						transferTypeInfo.mClassInfo = &handle->mClassInfo;

						// Extract the storage
						if( handle->HasStorage() == false )
						{
							RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION,
								"Found a pre-created local instance when"
								" trying to resolve an unlinked indirection,"
								" but it appears to not be relocatable" );
							return false;
						}
						UniquePtr<void> transferValue = handle->ExtractStorage();
						RF_ASSERT( handle->GetStrongestAddress() == transferValue );

						// IMPORTANT: It is possible (but unlikely) that a
						//  successful transfer operation will destroy the
						//  memory, so fixup may need to occur
						void const* const transferMemoryDestructionCanary = transferValue;

						// Transfer it
						bool const writeSuccess =
							accessor.mInsertVariableViaUPtr(
								accessorLocation,
								rftl::move( keyCopy ),
								keyInfo,
								rftl::move( transferValue ),
								transferTypeInfo );
						if( writeSuccess == false )
						{
							RFLOG_NOTIFY( fullChain, RFCAT_SERIALIZATION,
								"Failed to transfer local indirection value to"
								" accessor target" );
							return false;
						}

						if( transferMemoryDestructionCanary != handle->GetStrongestAddress() )
						{
							RF_ASSERT( handle->GetStrongestAddress() == nullptr );
							RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION,
								"While resolving an unlinked indirection, a"
								" handle to transferred memory was wiped out,"
								" and the re-fetch code post-transfer is not"
								" yet implemented" );
							RF_TODO_BREAK_MSG(
								"Perform a re-fetch after transfer?" );
							RF_TODO_ANNOTATION(
								"Might be able to avoid this entirely by doing"
								" deferred linking instead" );
							return false;
						}
					}
					else if( current.mUnlinkedIndirectionData->mExternalReferenceID.has_value() )
					{
						// External reference

						RF_ASSERT( current.mUnlinkedIndirectionData->mLocalInstanceID.has_value() == false );

						rftl::string const& referenceID =
							current.mUnlinkedIndirectionData->mExternalReferenceID.value();

						RF_TODO_BREAK_MSG( "Implement external reference support" );
						RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "External references not supported yet" );
						( (void)referenceID );
						return false;
					}
					else
					{
						RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION,
							"Could not determine how to handle an unlinked indirection" );
						RF_DBGFAIL();
						return false;
					}
				}
				else
				{
					// Construct a placeholder instance and insert it

					RF_TODO_BREAK_MSG(
						"Uh... What do do in this case? We need to create the"
						" variable ourselves? Do we need the type ID?" );
					RF_TODO_BREAK_MSG(
						"Is it possible / practical to implement this?" );

					// HACK: Some sketching out of some possible code that is
					//  based on how the OOLoader figures this out when it has
					//  to instantiate from Squirrel files, but those have the
					//  advantage of encoding the type information, which at
					//  time of writing is not stored at the property, in favor
					//  of using indirections instead (to allow for more
					//  complex pointer graphs than what the OOLoader can
					//  handle with all its limitations)
					static constexpr bool kTheoreticalStubCode = false;
					if constexpr( kTheoreticalStubCode )
					{
						rftl::string const instanceClassName = {}; //vm.GetNestedInstanceClassName( targetPath );
						if( instanceClassName.empty() )
						{
							RFLOG_NOTIFY( fullChain, RFCAT_SERIALIZATION, "Failed to determine class name for instance to construct" );
							return false;
						}

						rftype::ConstructedType constructed = {}; //ConstructClassByClassName( scratchLookup, instanceClassName );
						if( constructed.mLocation == nullptr )
						{
							RFLOG_NOTIFY( fullChain, RFCAT_SERIALIZATION, "Failed to construct instance" );
							return false;
						}
						RF_ASSERT( constructed.mClassInfo != nullptr );

						// Copy the key into a UniquePtr to transfer it
						RF_TODO_BREAK();
						//static_assert( rftl::is_same<decltype( key ), size_t const>::value );
						UniquePtr<size_t> keyCopy = nullptr; //DefaultCreator<size_t>::Create( key );

						reflect::VariableTypeInfo valueInfo = {};
						valueInfo.mClassInfo = constructed.mClassInfo;

						bool const writeSuccess =
							accessor.mInsertVariableViaUPtr(
								accessorLocation,
								rftl::move( keyCopy ),
								keyInfo,
								rftl::move( constructed.mLocation ),
								valueInfo );
						if( writeSuccess == false )
						{
							RFLOG_NOTIFY( fullChain, RFCAT_SERIALIZATION, "Failed to transfer placeholder value to accessor target" );
							return false;
						}
					}

					RF_TODO_BREAK();
					return false;
				}
			}
			else
			{
				RFLOG_NOTIFY( fullChain, RFCAT_SERIALIZATION, "Failed to write any placeholder values to accessor target" );
				return false;
			}

			if( accessor.mGetMutableTargetByKey == nullptr )
			{
				// Doesn't support overwriting a variable in place
				RF_TODO_BREAK_MSG(
					"Uh... What do do in this case? We need to create the"
					" variable ourselves and stomp it all later when the"
					" target node closes? Do we need the type ID?" );
				return false;
			}

			void* target = nullptr;
			reflect::VariableTypeInfo targetInfo = {};
			bool const mutableSuccess = accessor.mGetMutableTargetByKey( accessorLocation, key, keyInfo, target, targetInfo );
			if( mutableSuccess == false )
			{
				RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Failed to get mutable target" );
				RF_DBGFAIL();
				return false;
			}

			// Start new target
			PendingAccessorTargetData& pendingTarget = accessorNode.mPendingTarget.emplace();
			pendingTarget.mTargetTypeInfo = targetInfo;

			RF_ASSERT( current.mVariableTypeInfo.has_value() == false );
			current.mVariableTypeInfo.emplace( pendingTarget.mTargetTypeInfo );
			current.mVariableLocation = target;

			return true;
		}

		RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Unknown extension node identifier '%s'", nodeIdentifier.c_str() );
		RF_DBGFAIL();
		return false;
	}

	RF_TODO_ANNOTATION( "At time of writing, the implementation of complex types is undecided, and will affect this code possibility" );
	RF_TODO_BREAK_MSG( "Can maybe get here if a complex key type was a parent? Depends on how that's implemented?" );
	RF_TODO_BREAK_MSG( "Can maybe get here if a complex target type was a parent? Depends on how that's implemented?" );

	RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Unexpected codepath, can't determine variable type of predecessors" );
	RF_DBGFAIL();
	return false;
}



bool VerifyTypeIDOfNewInstance( ScratchSpace const& scratch, exporter::TypeID const& typeID )
{
	// There should be an instance node that is a class type, and a null
	//  placeholder for the first property node after that
	RF_ASSERT( scratch.mWalkChain.size() == 2 );
	RF_ASSERT( scratch.mWalkChain.at( 1 ) == nullptr );
	UniquePtr<WalkNode> const& instanceNodePtr = scratch.mWalkChain.at( 0 );
	RF_ASSERT( instanceNodePtr != nullptr );
	WalkNode const& instanceNode = *instanceNodePtr;
	RF_ASSERT( instanceNode.mVariableTypeInfo.has_value() );
	reflect::VariableTypeInfo const& typeInfo = *instanceNode.mVariableTypeInfo;
	RF_ASSERT( typeInfo.mClassInfo != nullptr );
	reflect::ClassInfo const& classInfo = *typeInfo.mClassInfo;

	if( scratch.mParams.mTypeLookupFunc == nullptr )
	{
		// No lookup support

		if( scratch.mParams.mContinueOnMissingTypeLookups )
		{
			// Ignore
			return true;
		}

		RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
			"Types found in data, but no type lookup support provided" );
		return false;
	}

	// Lookup the type
	static_assert( rftl::is_same<exporter::TypeID, math::HashVal64>::value );
	reflect::ClassInfo const* expectedClassInfo = scratch.mParams.mTypeLookupFunc( typeID );
	if( expectedClassInfo == nullptr )
	{
		// Not found

		if( scratch.mParams.mContinueOnMissingTypeLookups )
		{
			// Ignore
			return true;
		}

		RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
			"Type lookup could not find the type from the data" );
		return false;
	}

	// Verify it matches
	if( &classInfo != expectedClassInfo )
	{
		RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
			"Type lookup resolved to a different type than what is being"
			" serialized to" );
		return false;
	}

	return true;
}



bool CreateDeferredInstanceOnWalkChain( ScratchSpace& scratch )
{
	if( scratch.mWalkChain.empty() == false )
	{
		RF_DBGFAIL_MSG( "Expect chain to be empty for deferred instances" );
		return false;
	}

	if( scratch.mDeferredInstance.has_value() == false )
	{
		RF_DBGFAIL_MSG( "Instance wasn't deferred properly" );
		return false;
	}

	// Pull off the deferred attributes
	DeferredInstance const deferredInstance = scratch.mDeferredInstance.value();
	scratch.mDeferredInstance = rftl::nullopt;

	// Create the root node on the chain that we need to prepare
	scratch.mWalkChain.emplace_back( DefaultCreator<WalkNode>::Create( kThis ) );
	WalkNode& root = *scratch.mWalkChain.back();

	// HACK: Use a one-time initialization
	// TODO: A more sophisticated TOC-based setup that only falls back on
	//  this when it has to
	if( scratch.mOneTimeSingleRootTOCOverride_Ref == nullptr )
	{
		RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
			"One-time single-object reference missing, this is not yet"
			" properly supported" );
		RF_TODO_BREAK_MSG( "Fix this whole pile of hacks" );
		return false;
	}
	{
		WeakPtr<ObjectInstance const> handlePtr = nullptr;
		static constexpr bool kUseOneTimeInstance = true;
		if constexpr( kUseOneTimeInstance )
		{
			// Get the one-time instance
			handlePtr = scratch.mOneTimeSingleRootTOCOverride_Ref;
			scratch.mOneTimeSingleRootTOCOverride_Ref = nullptr;
		}
		else
		{
			ScratchObjectStorage& scratchObjectStorage = scratch.mScratchObjectStorage;

			// Get the next non-ID'd instance in queue
			UniquePtr<ObjectInstance> temp =
				rftl::move( scratchObjectStorage.mObjectInstancesWithoutIDs_Fresh.front() );
			RF_ASSERT( temp != nullptr );
			scratchObjectStorage.mObjectInstancesWithoutIDs_Fresh.pop_front();

			// Move it to the filled-out queue as an in-progress instance
			handlePtr = temp;
			scratchObjectStorage.mObjectInstancesWithoutIDs_FilledOut.push_back(
				rftl::move( temp ) );
		}
		RF_ASSERT( handlePtr != nullptr );
		ObjectInstance const& handle = *handlePtr;

		reflect::VariableTypeInfo storage = {};
		storage.mClassInfo = &handle.mClassInfo;
		root.mVariableTypeInfo.emplace( storage );
		root.mVariableLocation = handle.GetStrongestAddress();
	}

	// Placeholder, waiting for first property
	scratch.mWalkChain.emplace_back( nullptr );

	// Check the type ID, if that information is present
	if( deferredInstance.mTypeID.has_value() )
	{
		exporter::TypeID const& typeID = deferredInstance.mTypeID.value();
		bool const correctTypeID = VerifyTypeIDOfNewInstance( scratch, typeID );
		if( correctTypeID == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"New instance has a mistmatched type ID" );
			return false;
		}
	}

	return true;
}

}
///////////////////////////////////////////////////////////////////////////////

bool ObjectDeserializer::DeserializeSingleObject(
	Importer& importer,
	reflect::ClassInfo const& classInfo,
	void* classInstance )
{
	Params params = {};

	// Not having type lookup support means we need to have the objects
	//  instantiated for us already, which is true in this case for the root,
	//  but might not be true for other objects if there's things like
	//  indirections, or other such complex cases
	( (void)params.mTypeLookupFunc );

	// Ignore where possible, but indirections will likely cause failures
	( (void)params.mContinueOnMissingTypeLookups );

	return DeserializeSingleObject(
		importer,
		classInfo,
		classInstance,
		params );
}



bool ObjectDeserializer::DeserializeSingleObject(
	Importer& importer,
	reflect::ClassInfo const& classInfo,
	void* classInstance,
	Params const& params )
{
	RF_ASSERT( classInstance != nullptr );

	Params wrappedParams = params;

	// Want to apply our override once
	bool hasBeenApplied = false;

	wrappedParams.mTocInstanceOverrideFunc =
		[&params, &classInfo, &classInstance, &hasBeenApplied]( exporter::InstanceID const& instanceID, rftl::optional<exporter::TypeID> const& typeID )
		-> rftl::optional<ObjectInstance>
	{
		// Give the caller a chance to override first
		if( params.mTocInstanceOverrideFunc != nullptr )
		{
			rftl::optional<ObjectInstance> retVal = params.mTocInstanceOverrideFunc(
				instanceID,
				typeID );
			if( retVal.has_value() )
			{
				return retVal;
			}
		}

		if( hasBeenApplied )
		{
			// Already applied, let logic continue as normal
			return rftl::nullopt;
		}

		if( typeID.has_value() )
		{
			RF_TODO_ANNOTATION( "What if this doesn't match the caller type?" );
		}

		hasBeenApplied = true;
		return rftl::optional<ObjectInstance>(
			rftl::in_place,
			classInfo,
			classInstance );
	};

	bool const success =
		DeserializeMultipleObjects( importer, 0, wrappedParams );
	if( success == false )
	{
		return false;
	}

	if( hasBeenApplied == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
			"Deserialization claimed success, but single-object in-place"
			" deserialization was requested, and the provided single root"
			" object was never hooked" );
		return false;
	}

	return true;
}



bool ObjectDeserializer::DeserializeMultipleObjects(
	Importer& importer,
	int TODO_Output,
	Params const& params )
{
	// Scratch space to simplify / optimize lambda captures
	details::ScratchSpace scratch( params );

	auto const onScopeEnd = OnScopeEnd(
		[&scratch]()
		{
			details::DestroyChain( scratch.mWalkChain );
		} );

	using InstanceID = Importer::InstanceID;
	using TypeID = Importer::TypeID;
	using IndirectionID = Importer::IndirectionID;
	using ExternalReferenceID = Importer::ExternalReferenceID;

	Importer::Callbacks callbacks = {};


	callbacks.mPreload_TableOfContentsEntry =
		[&scratch]( InstanceID const& instanceID, rftl::optional<TypeID> const& typeID ) -> bool
	{
		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "TOC entry" );

		// Goal is to use the TOC entries to pre-create instances, and put them
		//  in a holding area waiting to be filled out, or appropriately
		//  slotted into an appropriate indirection.
		// If the types aren't present in the TOC, that's optional, but means
		//  that the caller has to help figure that out later, which might be
		//  okay if it's a single object or an array.
		// If the TOC isn't provided, that's optional too, but has similar
		//  implications to not having type information.

		// If an importer is going to go through the trouble to emit TOC
		//  entries, then they should atleast have valid instance IDs, even if
		//  they're not coming from data and are just generated on-the-fly
		//  using simple incrementing
		if( instanceID == exporter::kInvalidInstanceID )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Invalid instance ID found in table of contents entries" );
			return false;
		}

		// Make note of the TOC entry
		{
			bool const newEntry = scratch.mTOCEntries.emplace( instanceID, typeID ).second;
			if( newEntry == false )
			{
				RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
					"Duplicate instance IDs found in table of contents entries" );
				return false;
			}
		}

		// Check for an override, such as when the caller wants to do an
		//  in-place deserialization into one or more existing objects
		if( scratch.mParams.mTocInstanceOverrideFunc != nullptr )
		{
			rftl::optional<ObjectInstance> override =
				scratch.mParams.mTocInstanceOverrideFunc( instanceID, typeID );
			if( override.has_value() )
			{
				// Extract
				UniquePtr<ObjectInstance> newInstance =
					DefaultCreator<ObjectInstance>::Create(
						rftl::move( override.value() ) );

				// HACK: Support the one-time non-constructing deserialization
				//  root case
				RF_TODO_ANNOTATION( "Re-evaluate how to do this" );
				RF_ASSERT( scratch.mOneTimeSingleRootTOCOverride_Ref == nullptr );
				scratch.mOneTimeSingleRootTOCOverride_Ref = newInstance;

				// Store the instance by ID
				details::ScratchObjectStorage& scratchObjectStorage =
					scratch.mScratchObjectStorage;
				bool const newEntry =
					scratchObjectStorage.mObjectInstancesByID.emplace( instanceID, rftl::move( newInstance ) ).second;
				RF_ASSERT_MSG( newEntry,
					"Instance ID collision, this should've been prevented by"
					" checking the TOC IDs for duplicates first, as that should be"
					" the only way this list is populated at time of writing" );
				return true;
			}
		}

		// Hopefully it has type information, but this is optional, to allow
		//  simple cases to reduce the complexity and size of data they use
		// EXAMPLE: Simple messaging protocol serialization, that might use
		//  arrays and whatnot, but won't use complex pointer graphs or fancy
		//  polymorphic types
		if( typeID.has_value() == false )
		{
			if( scratch.mParams.mContinueOnMissingTypeLookups )
			{
				// Ignore, but may lead to an error during later logic
				return true;
			}

			RFLOG_WARNING( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"A TOC entry is missing type information, and type lookups are"
				" not set to continue on failure, this might be a sign of"
				" error, where sparse data is being passed to strict code" );
			RF_TODO_BREAK_MSG(
				"Probably want to have a mechanism for dynamic lists of root"
				" objects to load up, which might need something like a"
				" caller-provided type-info fallback callback" );
			return true;
		}
		TypeID const& typeIDVal = typeID.value();

		// Need to get the class info from the type ID
		if( scratch.mParams.mTypeLookupFunc == nullptr )
		{
			if( scratch.mParams.mContinueOnMissingTypeLookups )
			{
				// Ignore, but may lead to an error during later logic
				return true;
			}

			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"No type lookup helper provided, cannot" );
			return false;
		}
		reflect::ClassInfo const* const classInfoPtr = scratch.mParams.mTypeLookupFunc( typeIDVal );
		if( classInfoPtr == nullptr )
		{
			if( scratch.mParams.mContinueOnMissingTypeLookups )
			{
				// Ignore, but may lead to an error during later logic
				return true;
			}

			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Type lookup failed" );
			return false;
		}
		reflect::ClassInfo const& classInfo = *classInfoPtr;

		// Need to be able to construct the object to store in the scratch
		//  space for deserializing into
		if( scratch.mParams.mClassConstructFunc == nullptr )
		{
			RF_ASSERT( scratch.mParams.mTypeLookupFunc != nullptr );

			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"No construction helper, cannot create instances from TOC" );
			return false;
		}
		rftype::ConstructedType constructed = scratch.mParams.mClassConstructFunc( classInfo );
		if( constructed.mLocation == nullptr )
		{
			RF_ASSERT( constructed.mClassInfo == nullptr );

			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Failed to construct type" );
			return false;
		}
		RF_ASSERT( constructed.mClassInfo != nullptr );

		// Store the instance by ID
		{
			UniquePtr<ObjectInstance> newInstance =
				DefaultCreator<ObjectInstance>::Create(
					*constructed.mClassInfo,
					rftl::move( constructed.mLocation ) );
			details::ScratchObjectStorage& scratchObjectStorage =
				scratch.mScratchObjectStorage;
			bool const newEntry =
				scratchObjectStorage.mObjectInstancesByID.emplace( instanceID, rftl::move( newInstance ) ).second;
			RF_ASSERT_MSG( newEntry,
				"Instance ID collision, this should've been prevented by"
				" checking the TOC IDs for duplicates first, as that should be"
				" the only way this list is populated at time of writing" );
		}

		return true;
	};


	callbacks.mRoot_BeginNewInstanceFunc =
		[&scratch]() -> bool
	{
		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "New instance" );

		scratch.mInstanceCount++;
		if( scratch.mInstanceCount > 1 )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Multiple instances encountered during single object"
				" deserialization, these will be skipped" );
			return false;
		}

		// It's possible to have an empty instance, which means the next
		//  instance would show up and potentially skip the deferred creation,
		//  so we check for that here as a last-chance fallback
		if( scratch.mDeferredInstance.has_value() )
		{
			RF_TODO_BREAK_MSG( "Untested - Step through this, verify logic is correct" );

			RF_ASSERT( scratch.mWalkChain.size() == 0 );

			bool const createSuccess = details::CreateDeferredInstanceOnWalkChain( scratch );
			if( createSuccess == false )
			{
				RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
					"Deferred instance creation failed" );
				return false;
			}

			// There should be an instance node, and a null placeholder for the
			//  first property node after that
			RF_ASSERT( scratch.mWalkChain.size() == 2 );
			RF_ASSERT( scratch.mWalkChain.at( 1 ) == nullptr );
			UniquePtr<details::WalkNode> const& instanceNodePtr = scratch.mWalkChain.at( 0 );
			RF_ASSERT( instanceNodePtr != nullptr );

			RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Deferred instance (new instance, empty previous?)" );
		}

		// Destroy the chain in preparation to start the next instance
		details::DestroyChain( scratch.mWalkChain );

		// Not actually creating the instance yet, we need to defer putting the
		//  instance into the chain until the last possible moment (usually on
		//  the first time a property is started on it), so that we have a
		//  chance to get an instance ID and a type ID, which could let us use
		//  a pre-created instance, or ask the caller for an override if we
		//  think we need to construct the object ourselves (the override is
		//  generally used by the single-object wrapper, so it could provide
		//  its one-time in-place override in the event there wasn't a TOC
		//  entry for it to have gotten it in already)
		( (void)&details::CreateDeferredInstanceOnWalkChain );
		RF_ASSERT( scratch.mDeferredInstance.has_value() == false );
		scratch.mDeferredInstance.emplace();

		return true;
	};


	callbacks.mRoot_RegisterLocalIndirectionFunc =
		[&scratch]( IndirectionID const& indirectionID, InstanceID const& instanceID ) -> bool
	{
		RF_ASSERT( instanceID != exporter::kInvalidInstanceID );

		if( scratch.mParams.mAllowLocalIndirections == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Encountered a local indirection, but the support for them is disabled" );
			return false;
		}

		bool const newEntry = scratch.mLocalIndirections.emplace( indirectionID, instanceID ).second;
		if( newEntry == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Duplicate indirection IDs found in local indirections entries" );
			return false;
		}

		return true;
	};


	callbacks.mRoot_RegisterExternalIndirectionFunc =
		[&scratch]( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID ) -> bool
	{
		RF_ASSERT( referenceID != nullptr );

		if( scratch.mParams.mAllowExternalIndirections == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Encountered an external indirection, but the support for them is disabled" );
			return false;
		}

		bool const newEntry = scratch.mExternalIndirections.emplace( indirectionID, referenceID ).second;
		if( newEntry == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Duplicate indirection IDs found in external indirections entries" );
			return false;
		}

		return true;
	};


	callbacks.mInstance_AddInstanceIDAttributeFunc =
		[&scratch]( InstanceID const& instanceID ) -> bool
	{
		RF_ASSERT( instanceID != exporter::kInvalidInstanceID );

		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply instance ID %llu", instanceID );

		// Note the instance ID for deferred instance creation to refer to
		RF_ASSERT( scratch.mDeferredInstance.has_value() );
		RF_ASSERT( scratch.mDeferredInstance->mInstanceID == exporter::kInvalidInstanceID );
		scratch.mDeferredInstance->mInstanceID = instanceID;

		return true;
	};


	callbacks.mInstance_AddTypeIDAttributeFunc =
		[&scratch]( TypeID const& typeID, char const* debugName ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply type ID %llu ('%s')", typeID, debugName );

		// Note the type ID for deferred instance creation to refer to
		RF_ASSERT( scratch.mDeferredInstance.has_value() );
		RF_ASSERT( scratch.mDeferredInstance->mTypeID.has_value() == false );
		scratch.mDeferredInstance->mTypeID = typeID;

		return true;
	};


	callbacks.mInstance_BeginNewPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "New property" );

		if( scratch.mWalkChain.size() == 0 )
		{
			bool const createSuccess = details::CreateDeferredInstanceOnWalkChain( scratch );
			if( createSuccess == false )
			{
				RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
					"Deferred instance creation failed" );
				return false;
			}

			// There should be an instance node, and a null placeholder for the
			//  first property node after that
			RF_ASSERT( scratch.mWalkChain.size() == 2 );
			RF_ASSERT( scratch.mWalkChain.at( 1 ) == nullptr );
			UniquePtr<details::WalkNode> const& instanceNodePtr = scratch.mWalkChain.at( 0 );
			RF_ASSERT( instanceNodePtr != nullptr );

			RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Deferred instance (new property)" );
		}

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );
		bool const replaceSuccess = details::ReplaceEndOfChain(
			scratch.mWalkChain,
			DefaultCreator<details::WalkNode>::Create( details::kUnset ) );
		if( replaceSuccess == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Node replacement failed" );
			return false;
		}

		return true;
	};


	callbacks.mProperty_AddNameAttributeFunc =
		[&scratch]( rftl::string_view const& name ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply name '%s'", RFTLE_CSTR( name ) );

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );
		scratch.mWalkChain.back()->mIdentifier = name;

		// NOTE: Could potentially resolve here, but there's no actual attempt
		//  to perform a meaningful action yet, and it's possible that a
		//  resolve would fail due to needing more information than just the
		//  name (an example would be an accessor target that can't be default
		//  constructed, and thus needs type information attribute data to
		//  perform the construction and insertion)

		return true;
	};


	callbacks.mProperty_AddValueAttributeFunc =
		[&scratch]( reflect::Value const& value ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply value '%s' [%s]", value.GetStoredTypeName(), rftl::to_string( value.GetBytes(), 16 ).c_str() );

		// At time of writing, it is expected resolving the current property
		//  has been deferred, so that it could build up any name or type
		//  information it needs first, and so will need to be attempted now
		RF_ASSERT( details::IsWalkChainLeadingEdgeResolved( scratch.mWalkChain ) == false );
		bool const resolveSuccess =
			details::ResolveWalkChainLeadingEdge(
				scratch.mWalkChain,
				scratch.mScratchObjectStorage );
		if( resolveSuccess == false )
		{
			// Normally, we'd want to bail, but there might be development
			//  reasons for allowing soft upgrades or whatnot?
			RF_TODO_BREAK_MSG( "Should this bail, or be resilient to non-existant properties? Options to control this?" );
			return false;
		}
		RF_ASSERT( details::IsWalkChainLeadingEdgeResolved( scratch.mWalkChain ) );

		bool const setSuccess = details::SetValueToChain( scratch.mWalkChain, value );
		if( setSuccess == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Failed to set value" );
			return false;
		}

		return true;
	};


	callbacks.mProperty_AddIndirectionAttributeFunc =
		[&scratch]( IndirectionID const& indirectionID ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply indirection %llu", indirectionID );

		bool const someIndirectionsSupported =
			scratch.mParams.mAllowLocalIndirections ||
			scratch.mParams.mAllowExternalIndirections;
		if( someIndirectionsSupported == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Encountered an indirection attribute, but the support for them is disabled" );
			return false;
		}

		// Is it local?
		rftl::optional<InstanceID> localIndirection = rftl::nullopt;
		{
			details::ScratchSpace::LocalIndirections::const_iterator const iter =
				scratch.mLocalIndirections.find( indirectionID );
			if( iter != scratch.mLocalIndirections.end() )
			{
				localIndirection = iter->second;
			}
		}

		// Is it external?
		rftl::optional<rftl::string_view> externalIndirection = rftl::nullopt;
		{
			details::ScratchSpace::ExternalIndirections::const_iterator const iter =
				scratch.mExternalIndirections.find( indirectionID );
			if( iter != scratch.mExternalIndirections.end() )
			{
				externalIndirection = iter->second;
			}
		}

		// Check that there's one of the options, and only one
		if( localIndirection.has_value() == false && externalIndirection.has_value() == false )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Encountered an indirection attribute, but the indirection ID"
				" isn't registered as either local OR external" );
			return false;
		}
		if( localIndirection.has_value() && externalIndirection.has_value() )
		{
			RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
				"Encountered an indirection attribute, but the indirection ID"
				" is registered as both local AND external" );
			return false;
		}

		if( localIndirection.has_value() )
		{
			// Local
			InstanceID const& instanceID = localIndirection.value();
			RF_ASSERT( instanceID != exporter::kInvalidInstanceID );

			if( scratch.mParams.mAllowLocalIndirections == false )
			{
				RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
					"Encountered an indirection attribute, but it's a local"
					" indirection and the support for them is disabled" );
				return false;
			}

			// Expect that we're currently at an unlinked node, that will need
			//  some more information to resolve properly in the chain
			RF_ASSERT( scratch.mWalkChain.size() > 1 );
			UniquePtr<details::WalkNode> const& currentNodePtr = scratch.mWalkChain.back();
			RF_ASSERT( currentNodePtr != nullptr );
			details::WalkNode& currentNode = *currentNodePtr;
			RF_ASSERT_MSG( currentNode.mVariableTypeInfo.has_value() == false,
				"Currently assume indirections don't have type info until linkage" );
			RF_ASSERT( currentNode.mVariableLocation == nullptr );

			// Pass along the indirection information
			RF_ASSERT( currentNode.mUnlinkedIndirectionData.has_value() == false );
			currentNode.mUnlinkedIndirectionData.emplace();
			currentNode.mUnlinkedIndirectionData->mLocalInstanceID = instanceID;

			// Attempt to resolve the chain, which at time of writing is
			//  expected to perform the linkage immediately (later
			//  implementations may support deferring this)
			RF_ASSERT( details::IsWalkChainLeadingEdgeResolved( scratch.mWalkChain ) == false );
			bool const resolveSuccess =
				details::ResolveWalkChainLeadingEdge(
					scratch.mWalkChain,
					scratch.mScratchObjectStorage );
			if( resolveSuccess == false )
			{
				RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
					"Failed to resolve an indirection on the walk-chain, this"
					" hopefully is preceded by more useful errors" );
				return false;
			}
			RF_ASSERT( details::IsWalkChainLeadingEdgeResolved( scratch.mWalkChain ) );

			return true;
		}
		else
		{
			// External
			RF_ASSERT( externalIndirection.has_value() );
			rftl::string_view const& externalReference = externalIndirection.value();

			if( scratch.mParams.mAllowExternalIndirections == false )
			{
				RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
					"Encountered an indirection attribute, but it's an"
					" external indirection and the support for them is"
					" disabled" );
				return false;
			}

			// Unclear what to do here, as the deserializer isn't going to be
			//  able to resolve this on its own, and may require some more
			//  complex heavy-lifting to be done by that caller, through
			//  callbacks passed along in the params, so that the caller can go
			//  out to fetch the necessary dependencies, though that should
			//  probably happen much earlier, when the initial indirections
			//  come in before the real loading starts, instead of here during
			//  the attribute step, since that's really quite late
			RF_TODO_BREAK_MSG( "Support for external indirections" );
			( (void)externalReference );
			return true;
		}
	};


	callbacks.mProperty_IndentFromCurrentPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Indent" );

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );

		// At time of writing, it is expected resolving the current property
		//  has been deferred, so that it could build up any name or type
		//  information it needs first, and so will need to be attempted now
		RF_ASSERT( details::IsWalkChainLeadingEdgeResolved( scratch.mWalkChain ) == false );
		bool const resolveSuccess =
			details::ResolveWalkChainLeadingEdge(
				scratch.mWalkChain,
				scratch.mScratchObjectStorage );
		if( resolveSuccess == false )
		{
			// Normally, we'd want to bail, but there might be development
			//  reasons for allowing soft upgrades or whatnot?
			RF_TODO_BREAK_MSG(
				"Should this bail, or be resilient to non-existant properties?"
				" Options to control this? Do we need to suppress indentation"
				" past the first failed resolve, and track when we return back"
				" to that level?" );
			return false;
		}
		RF_ASSERT( details::IsWalkChainLeadingEdgeResolved( scratch.mWalkChain ) );

		// Placeholder, waiting for first property
		scratch.mWalkChain.emplace_back( nullptr );

		return true;
	};


	callbacks.mProperty_OutdentFromLastIndentFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Outdent" );

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );
		details::PopFromChain( scratch.mWalkChain );

		return true;
	};


	bool const success = importer.ImportAndFinalize( callbacks );
	if( success == false )
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

template<>
void RF::logging::WriteContextString( RF::serialization::details::WalkChain const& context, Utf8LogContextBuffer& buffer )
{
	size_t bufferOffset = 0;
	size_t const maxBufferOffset = buffer.size();

	size_t const numElements = context.size();
	for( size_t i = 0; i < numElements; i++ )
	{
		RF::serialization::details::WalkChain::value_type const& element = context.at( i );

		if( bufferOffset >= maxBufferOffset )
		{
			break;
		}
		if( i != 0 )
		{
			buffer[bufferOffset] = '/';
			bufferOffset++;
		}
		if( bufferOffset >= maxBufferOffset )
		{
			break;
		}

		rftl::string_view toWrite = {};
		if( element == nullptr )
		{
			toWrite = "!NULL";
		}
		else
		{
			toWrite = element->mIdentifier;
			if( toWrite.empty() )
			{
				toWrite = "!UNSET";
			}
		}

		size_t const bytesRemaining = maxBufferOffset - bufferOffset;
		size_t const bytesToWrite = math::Min( bytesRemaining, toWrite.size() );
		memcpy( &buffer[bufferOffset], toWrite.data(), bytesToWrite );
		bufferOffset += bytesToWrite;
	}
}
