#include "stdafx.h"
#include "ObjectDeserializer.h"

#include "Logging/Logging.h"
#include "Serialization/Importer.h"

#include "core_math/math_clamps.h"
#include "core_rftype/TypeTraverser.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

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



struct WalkNode
{
	RF_NO_COPY( WalkNode );

	WalkNode() = default;

	WalkNode( rftl::string&& identifier )
		: mIdentifier( rftl::move( identifier ) )
	{
		//
	}

	rftl::string mIdentifier;

	reflect::VariableTypeInfo const* mVariableTypeInfo = nullptr;
	void const* mVariableLocation = nullptr;

	rftl::optional<PendingAccessorKeyData> mPendingKey;
	UniquePtr<WalkNode> mRecentKey = nullptr;

	// If on an accessor, this indicates what index we're on
	size_t numKeyStartsObserved = 0;
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



bool TryPopFromChain( WalkChain& fullChain )
{
	RF_ASSERT( fullChain.empty() == false );

	UniquePtr<WalkNode> extracted = rftl::move( fullChain.back() );
	fullChain.pop_back();

	// TODO: Handle extracted node, if it's related to a parent accessor then
	//  it needs to checked against the parent, and potentially inserted into
	//  the accessor if it has a key+target pairing

	return true;
}



bool ResolveWalkChainLeadingEdge( WalkChain& fullChain )
{
	if( fullChain.size() < 2 )
	{
		RF_DBGFAIL_MSG( "Can't resolve a chain without multiple nodes" );
		return false;
	}

	WalkNode const& first = *fullChain.front();
	WalkNode& previous = **( fullChain.rbegin() + 1 );
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
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Attempting resolve on a value" );
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
			current.mVariableLocation = reinterpret_cast<uint8_t const*>( previous.mVariableLocation ) + varInfo.mOffset;
			return true;
		}

		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Could not find member '%s'", memberNameToFind.c_str() );
		return false;
	}

	// Child of an accessor?
	if( previous.mVariableTypeInfo->mAccessor != nullptr )
	{
		WalkNode& accessorNode = previous;
		reflect::ExtensionAccessor const& accessor = *previous.mVariableTypeInfo->mAccessor;
		void const* const accessorLocation = previous.mVariableLocation;
		RF_ASSERT( accessorLocation != nullptr );
		rftl::string const& nodeIdentifier = current.mIdentifier;

		if( nodeIdentifier == "K" )
		{
			// Key

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

			// Wipe out old key information
			accessorNode.mRecentKey = nullptr;
			accessorNode.mPendingKey.reset();

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
			RF_TODO_BREAK();
			return false;
		}

		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unknown extension node identifier '%s'", nodeIdentifier.c_str() );
		return false;
	}

	RF_TODO_ANNOTATION( "At time of writing, the implementation of complex types is undecided, and will affect this code possibility" );
	RF_TODO_BREAK_MSG( "Can maybe get here if a complex key type was a parent? Depends on how that's implemented?" );
	RF_TODO_BREAK_MSG( "Can maybe get here if a complex target type was a parent? Depends on how that's implemented?" );

	RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected codepath, can't determine variable type of predecessors" );
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

	using InstanceID = Importer::InstanceID;
	using TypeID = Importer::TypeID;
	using IndirectionID = Importer::IndirectionID;
	using ExternalReferenceID = Importer::ExternalReferenceID;

	Importer::Callbacks callbacks = {};


	callbacks.mRoot_BeginNewInstanceFunc =
		[&scratch]() -> bool
	{
		scratch.mInstanceCount++;
		if( scratch.mInstanceCount > 1 )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
				"Multiple instances encountered during single object"
				" deserialization, these will be skipped" );
			return false;
		}

		scratch.mWalkChain.clear();
		scratch.mWalkChain.emplace_back( DefaultCreator<details::WalkNode>::Create( details::kThis ) );
		details::WalkNode& root = *scratch.mWalkChain.back();
		root.mVariableTypeInfo = &scratch.mClassTypeInfo;
		root.mVariableLocation = scratch.mClassInstance;

		scratch.mWalkChain.emplace_back( DefaultCreator<details::WalkNode>::Create( details::kUnset ) );

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

		// Don't care, only support one object
		return true;
	};


	callbacks.mInstance_AddTypeIDAttributeFunc =
		[&scratch]( TypeID const& typeID, char const* debugName ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RF_TODO_ANNOTATION( "Make sure the type ID matches what we're expecting" );
		return true;
	};


	callbacks.mInstance_BeginNewPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );
		details::TryPopFromChain( scratch.mWalkChain );
		scratch.mWalkChain.emplace_back( DefaultCreator<details::WalkNode>::Create( details::kUnset ) );

		return true;
	};


	callbacks.mProperty_AddNameAttributeFunc =
		[&scratch]( rftl::string_view const& name ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

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

		RFLOG_WARNING( scratch.mWalkChain, RFCAT_SERIALIZATION, "TODO: Set value" );
		RF_TODO_BREAK_MSG( "Use some fancy path-based helper to set this" );

		return true;
	};


	callbacks.mProperty_AddIndirectionAttributeFunc =
		[&scratch]( IndirectionID const& indirectionID ) -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
			"Indirections not supported in single object deserialization" );
		return false;
	};


	callbacks.mProperty_IndentFromCurrentPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );
		scratch.mWalkChain.emplace_back( DefaultCreator<details::WalkNode>::Create( details::kUnset ) );

		return true;
	};


	callbacks.mProperty_OutdentFromLastIndentFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.mInstanceCount == 1 );

		RF_ASSERT( scratch.mWalkChain.size() >= 1 );
		details::TryPopFromChain( scratch.mWalkChain );

		return true;
	};


	bool const success = importer.ImportAndFinalize( callbacks );
	if( success == false )
	{
		return false;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
}

template<>
static void RF::logging::WriteContextString( RF::serialization::details::WalkChain const& context, Utf8LogContextBuffer& buffer )
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

		rftl::string_view toWrite = element->mIdentifier;
		if( toWrite.empty() )
		{
			toWrite = "!UNSET";
		}

		size_t const bytesRemaining = maxBufferOffset - bufferOffset;
		size_t const bytesToWrite = math::Min( bytesRemaining, toWrite.size() );
		memcpy( &buffer[bufferOffset], toWrite.data(), bytesToWrite );
		bufferOffset += bytesToWrite;
	}
}
