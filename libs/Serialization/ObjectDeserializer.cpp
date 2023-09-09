#include "stdafx.h"
#include "ObjectDeserializer.h"

#include "Logging/Logging.h"
#include "Serialization/Importer.h"

#include "core_math/math_clamps.h"
#include "core_rftype/TypeTraverser.h"

#include "core/meta/ScopedCleanup.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

RF_TODO_ANNOTATION( "Remove c_str usages, update logging to C++20 format" );
#include "rftl/extension/c_str.h"

#include "rftl/extension/string_format.h"
#include "rftl/optional"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr char kThis[] = "!this";
static constexpr char kUnset[] = "";



struct PendingAccessorKeyData
{
	// Will be pointed to by key nodes, so must persist until the key node has
	//  closed and returned back to the accessor
	reflect::VariableTypeInfo mVariableTypeInfoStorage = {};

	// Values don't need fancy construction, so can exist as just bytes
	rftl::vector<uint8_t> mValueStorage = {};

	RF_TODO_ANNOTATION( "Support for keys that aren't value types" );
};



struct PendingAccessorTargetData
{
	// Will be pointed to by target nodes, so must persist until the target
	//  node has closed and returned back to the accessor
	reflect::VariableTypeInfo mVariableTypeInfoStorage = {};

	// TODO: This applies to things that don't support default construction,
	//  and things that don't allow mutable access to their contents
	RF_TODO_ANNOTATION( "Support for accessors that need targets to be created externally" );
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

	reflect::VariableTypeInfo const* mVariableTypeInfo = nullptr;
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

	// If on an accessor, these are used to help determine what what index
	//  we're on
	size_t numKeyStartsObserved = 0;
	size_t numTargetStartsObserved = 0;
};

using WalkChain = rftl::vector<UniquePtr<WalkNode>>;



struct SingleScratchSpace
{
	RF_NO_COPY( SingleScratchSpace );

	SingleScratchSpace(
		reflect::ClassInfo const& classInfo,
		void* classInstance )
		: mClassInfo( classInfo )
		, mClassInstance( classInstance )
		, mClassTypeInfo( { reflect::Value::Type::Invalid, &classInfo, nullptr } )
	{
		//
	}

	reflect::ClassInfo const& mClassInfo;
	void* const mClassInstance;

	// Need this to seed the root of the walk chain, must remain addressable
	//  during the walk
	reflect::VariableTypeInfo const mClassTypeInfo;

	size_t mInstanceCount = 0;
	WalkChain mWalkChain;
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

		if( node.mVariableTypeInfo == nullptr )
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
	fullChain.pop_back();

	RF_ASSERT( fullChain.back() != nullptr );
	WalkNode& current = *fullChain.back();

	RF_TODO_ANNOTATION(
		"What about accessor keys and targets?"
		" Do they really require identifiers?" );
	RF_ASSERT( current.mIdentifier.empty() == false );
	RF_ASSERT( current.mVariableTypeInfo != nullptr );
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
			RF_ASSERT( &current.mPendingKey->mVariableTypeInfoStorage == keyNode.mVariableTypeInfo );
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
			RF_ASSERT( current.mRecentKey->mVariableTypeInfo == &current.mPendingKey->mVariableTypeInfoStorage );
			RF_ASSERT( current.mPendingTarget.has_value() );
			RF_ASSERT( targetNode.mVariableTypeInfo == &current.mPendingTarget->mVariableTypeInfoStorage );
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
	RF_ASSERT( current.mVariableTypeInfo != nullptr );
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



bool ResolveWalkChainLeadingEdge( WalkChain& fullChain )
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
	RF_ASSERT( first.mVariableTypeInfo != nullptr );
	RF_ASSERT( first.mVariableLocation != nullptr );
	RF_ASSERT( previous.mIdentifier.empty() == false );
	RF_ASSERT( previous.mVariableTypeInfo != nullptr );
	RF_ASSERT( previous.mVariableLocation != nullptr );
	RF_ASSERT( current.mIdentifier.empty() == false );
	RF_ASSERT( current.mVariableTypeInfo == nullptr );
	RF_ASSERT( current.mVariableLocation == nullptr );
	current.mVariableTypeInfo = nullptr;
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
			current.mVariableTypeInfo = &varInfo.mVariableTypeInfo;
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
			pendingKey.mVariableTypeInfoStorage.mValueType = keyTypeInfo.mValueType;
			pendingKey.mValueStorage.resize( reflect::Value::GetNumBytesNeeded( keyTypeInfo.mValueType ) );

			RF_ASSERT( current.mVariableTypeInfo == nullptr );
			current.mVariableTypeInfo = &pendingKey.mVariableTypeInfoStorage;
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
			RF_ASSERT( keyNode.mVariableTypeInfo != nullptr );
			void const* const& key = keyNode.mVariableLocation;
			reflect::VariableTypeInfo const& keyInfo = *keyNode.mVariableTypeInfo;

			if( accessor.mInsertVariableDefault == nullptr )
			{
				// Doesn't support creating a placeholder variable
				RF_TODO_BREAK_MSG(
					"Uh... What do do in this case? We need to create the"
					" variable ourselves? Do we need the type ID?" );
				return false;
			}

			bool const insertSuccess = accessor.mInsertVariableDefault( accessorLocation, key, keyInfo );
			if( insertSuccess == false )
			{
				RFLOG_ERROR( fullChain, RFCAT_SERIALIZATION, "Failed to insert key" );
				RF_DBGFAIL();
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
			pendingTarget.mVariableTypeInfoStorage = targetInfo;

			RF_ASSERT( current.mVariableTypeInfo == nullptr );
			current.mVariableTypeInfo = &pendingTarget.mVariableTypeInfoStorage;
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

}
///////////////////////////////////////////////////////////////////////////////

bool ObjectDeserializer::DeserializeSingleObject(
	Importer& importer,
	reflect::ClassInfo const& classInfo,
	void* classInstance )
{
	RF_ASSERT( classInstance != nullptr );

	details::SingleScratchSpace scratch( classInfo, classInstance );

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

		details::DestroyChain( scratch.mWalkChain );
		scratch.mWalkChain.emplace_back( DefaultCreator<details::WalkNode>::Create( details::kThis ) );
		details::WalkNode& root = *scratch.mWalkChain.back();
		root.mVariableTypeInfo = &scratch.mClassTypeInfo;
		root.mVariableLocation = scratch.mClassInstance;

		// Placeholder, waiting for first property
		scratch.mWalkChain.emplace_back( nullptr );

		return true;
	};


	callbacks.mRoot_RegisterLocalIndirectionFunc =
		[]( IndirectionID const& indirectionID, InstanceID const& instanceID ) -> bool
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
			"Indirections not supported in single object deserialization" );
		return false;
	};


	callbacks.mRoot_RegisterExternalIndirectionFunc =
		[]( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID ) -> bool
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
			"Indirections not supported in single object deserialization" );
		return false;
	};


	callbacks.mInstance_AddInstanceIDAttributeFunc =
		[&scratch]( InstanceID const& instanceID ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply instance ID %llu", instanceID );

		// Don't care, only support one object
		return true;
	};


	callbacks.mInstance_AddTypeIDAttributeFunc =
		[&scratch]( TypeID const& typeID, char const* debugName ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply type ID %llu ('%s')", typeID, debugName );

		RF_TODO_ANNOTATION( "Make sure the type ID matches what we're expecting" );
		return true;
	};


	callbacks.mInstance_BeginNewPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "New property" );

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

		bool const resolveSuccess = details::ResolveWalkChainLeadingEdge( scratch.mWalkChain );
		if( resolveSuccess == false )
		{
			// Normally, we'd want to bail, but there might be development
			//  reasons for allowing soft upgrades or whatnot?
			RF_TODO_BREAK_MSG( "Should this bail, or be resilient to non-existant properties? Options to control this?" );
			return false;
		}

		return true;
	};


	callbacks.mProperty_AddValueAttributeFunc =
		[&scratch]( reflect::Value const& value ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Apply value '%s' [%s]", value.GetStoredTypeName(), rftl::to_string( value.GetBytes(), 16 ).c_str() );

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

		RFLOG_ERROR( scratch.mWalkChain, RFCAT_SERIALIZATION,
			"Indirections not supported in single object deserialization" );
		return false;
	};


	callbacks.mProperty_IndentFromCurrentPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_DEBUG( scratch.mWalkChain, RFCAT_SERIALIZATION, "Indent" );

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );

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