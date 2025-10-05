#include "stdafx.h"
#include "ObjectSerializer.h"

#include "Serialization/Exporter.h"

#include "Logging/Logging.h"

#include "core_rftype/StoredClassKey.h"
#include "core_rftype/TypeTraverser.h"

#include "rftl/unordered_map"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

bool Instance_TryAddTypeIDAttribute(
	Exporter& exporter,
	ObjectSerializer::Params const& params,
	reflect::ClassInfo const& classInfo,
	void const* classInstance )
{
	if( params.mTypeLookupFunc == nullptr )
	{
		// Opted out of this functionality, treat as success
		return true;
	}

	// Lookup the key
	rftype::StoredClassKey const classKey = params.mTypeLookupFunc( classInfo );
	if( classKey.IsValid() == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION,
			"Failed to lookup class key for class instance during"
			" serialization, cannot encode type ID" );
		RF_DBGFAIL();
		RF_TODO_ANNOTATION( "Need to find some useful context to log here" );

		// Failed to find, how should this be treated?
		if( params.mContinueOnMissingTypeLookups )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// Attempt add
	return exporter.Instance_AddTypeIDAttribute(
		classKey.GetHash(),
		classKey.mName );
}



bool Property_TryAddDebugTypeIDAttribute(
	Exporter& exporter,
	ObjectSerializer::Params const& params,
	reflect::ClassInfo const& classInfo,
	void const* classInstance )
{
	if( params.mTypeLookupFunc == nullptr )
	{
		// Opted out of this functionality, treat as success
		return true;
	}

	// Lookup the key
	rftype::StoredClassKey const classKey = params.mTypeLookupFunc( classInfo );
	if( classKey.IsValid() == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION,
			"Failed to lookup class key for class instance during"
			" serialization, cannot encode type ID" );
		RF_DBGFAIL();
		RF_TODO_ANNOTATION( "Need to find some useful context to log here" );

		// Failed to find, how should this be treated?
		if( params.mContinueOnMissingTypeLookups )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// Attempt add
	return exporter.Property_AddDebugTypeIDAttribute(
		classKey.GetHash(),
		classKey.mName );
}



bool IsPolymorphicIndirection( reflect::IndirectionInfo const& indirectionInfo )
{
	bool const isMemoryLookup = indirectionInfo.mMemoryLookupPointerType.has_value();
	if( isMemoryLookup )
	{
		// Memory
		reflect::VariableTypeInfo const& pointerType = indirectionInfo.mMemoryLookupPointerType.value();

		bool const knowsInstance = indirectionInfo.mMemoryLookupInstanceType.has_value();
		if( knowsInstance )
		{
			// With an instance
			reflect::VariableTypeInfo const& instanceType = indirectionInfo.mMemoryLookupInstanceType.value();

			bool const instanceDiffersFromPointer = pointerType != instanceType;
			if( instanceDiffersFromPointer )
			{
				// With a divergance

				// Definitely polymorphic
				return true;
			}
		}

		bool const isClassType = pointerType.mClassInfo != nullptr;
		if( isClassType )
		{
			// Is a class
			reflect::ClassInfo const& pointerClassInfo = *pointerType.mClassInfo;

			bool const isKnownVirtualCapable = pointerClassInfo.mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor;
			if( isKnownVirtualCapable )
			{
				// COULD be polymorphic, but doesn't appear to be exercising
				//  the capability on this instance?

				// Return false? Is this the correct approach here?
				return false;
			}
		}
	}

	return false;
}

}
///////////////////////////////////////////////////////////////////////////////

bool ObjectSerializer::SerializeSingleObject(
	Exporter& exporter,
	reflect::ClassInfo const& classInfo,
	void const* classInstance )
{
	Params params = {};

	// Not writing an instance ID, assuming there's only one and that it can't
	//  have indirect references made to it
	( (void)params.mInstanceID );

	// Not writing any type IDs, since that requires more complicated machinery
	//  that we don't want to invoke so we can keep the single object
	//  serialization simple
	// NOTE: This means deserialization will need to have the root type made
	//  for it, so single object serialization will need some other way to know
	//  what type was serialized, such as filename extension or packet headers
	( (void)params.mTypeLookupFunc );

	// Try to collapse as much as we can, since we're not providing indirection
	//  mechanisms like ID generation
	params.mCollapseAllPossibleIndirections = true;

	// Should fail to be able to produce indirections, since we did not provide
	//  a way to generate the IDs for them
	// NOTE: Expect the serialize to have return as failed if it tried, before
	//  trying to use this callback
	( (void)params.mIndirectionDeferFunc );

	bool const success = SerializeSingleObject(
		exporter,
		classInfo,
		classInstance,
		params );

	return success;
}



bool ObjectSerializer::SerializeSingleObject(
	Exporter& exporter,
	reflect::ClassInfo const& classInfo,
	void const* classInstance,
	Params const& params )
{
	bool success;

	success = exporter.Root_BeginNewInstance();
	if( success == false )
	{
		return false;
	}

	RF_TODO_ANNOTATION(
		"Error handling for when operations return false, most code here"
		" silently assumes success and keeps going" );

	// Instance ID
	if( params.mInstanceID != exporter::kInvalidInstanceID )
	{
		exporter.Instance_AddInstanceIDAttribute( params.mInstanceID );
	}

	// Type ID
	details::Instance_TryAddTypeIDAttribute( exporter, params, classInfo, classInstance );

	auto onMemberVariable =
		[&exporter, &params](
			rftype::TypeTraverser::MemberVariableInstance const& memberVariableInstance )
		-> void
	{
		char const* const name = memberVariableInstance.mMemberVariableInfo.mIdentifier;
		reflect::VariableTypeInfo const& typeInfo = memberVariableInstance.mMemberVariableInfo.mVariableTypeInfo;
		void const* const location = memberVariableInstance.mMemberVariableLocation;
		exporter.Instance_BeginNewProperty();
		exporter.Property_AddNameAttribute( name );
		if( typeInfo.mValueType != reflect::Value::Type::Invalid )
		{
			// A value, expect to be a leaf element with no traversal
			RF_ASSERT( typeInfo.mClassInfo == nullptr );
			RF_ASSERT( typeInfo.mAccessor == nullptr );

			// Serialize immediately
			reflect::Value::Type const& type = typeInfo.mValueType;
			exporter.Property_AddValueAttribute( reflect::Value( type, location ) );
		}
		else if( typeInfo.mClassInfo != nullptr )
		{
			// A nested type, expect a traversal callback soon
			RF_ASSERT( typeInfo.mValueType == reflect::Value::Type::Invalid );
			RF_ASSERT( typeInfo.mAccessor == nullptr );

			// Note the type for debug purposes
			details::Property_TryAddDebugTypeIDAttribute(
				exporter,
				params,
				*typeInfo.mClassInfo,
				location );
		}
		else if( typeInfo.mAccessor != nullptr )
		{
			// An accessor, expect a traversal callback soon
			RF_ASSERT( typeInfo.mValueType == reflect::Value::Type::Invalid );
			RF_ASSERT( typeInfo.mClassInfo == nullptr );

			RF_TODO_ANNOTATION( "Information about what this is?" );
		}
		else
		{
			RF_DBGFAIL_MSG( "Unexpected codepath" );
		}
	};

	auto onTraversalTypeFound =
		[&exporter, &params, &success](
			rftype::TypeTraverser::TraversalType traversalType,
			rftype::TypeTraverser::TraversalVariableInstance const& varInst,
			bool& shouldRecurse )
		-> void
	{
		switch( traversalType )
		{
			case RF::rftype::TypeTraverser::TraversalType::NestedType:
			{
				// onMemberVariable(...) should've already prepared this node

				RF_ASSERT_MSG( varInst.mIndirectionInfo == nullptr,
					"Currently don't expect nested types to be indirect" );

				exporter.Property_IndentFromCurrentProperty();
				shouldRecurse = true;
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::Accessor:
			{
				// onMemberVariable(...) should've already prepared this node

				RF_ASSERT_MSG( varInst.mIndirectionInfo == nullptr,
					"Currently don't expect accessors to be indirect" );

				reflect::VariableTypeInfo const& typeInfo = varInst.mVariableTypeInfo;
				RF_ASSERT( typeInfo.mAccessor != nullptr );
				void const* const location = varInst.mVariableLocation;
				RF_ASSERT( location != nullptr );
				size_t const numVariables = typeInfo.mAccessor->mGetNumVariables( location );
				if( numVariables > 0 )
				{
					exporter.Property_IndentFromCurrentProperty();
					shouldRecurse = true;
				}
				else
				{
					// Don't recurse or indent if there's no variables, since
					//  all that would happen is an immediate outdent, which is
					//  just wasteful, and would generally be suspected as a
					//  bug when encountered
					shouldRecurse = false;
				}
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::AccessorKey:
			{
				RF_ASSERT_MSG( varInst.mIndirectionInfo == nullptr,
					"Currently don't expect accessor keys to be indirect" );

				char const* const keyName = "K"; // Key
				reflect::VariableTypeInfo const& typeInfo = varInst.mVariableTypeInfo;
				void const* const location = varInst.mVariableLocation;
				exporter.Instance_BeginNewProperty();
				exporter.Property_AddNameAttribute( keyName );
				if( typeInfo.mValueType != reflect::Value::Type::Invalid )
				{
					// A value key
					RF_ASSERT( typeInfo.mClassInfo == nullptr );
					RF_ASSERT( typeInfo.mAccessor == nullptr );

					// Serialize immediately
					reflect::Value::Type const& type = typeInfo.mValueType;
					exporter.Property_AddValueAttribute( reflect::Value( type, location ) );
				}
				else if( typeInfo.mClassInfo != nullptr )
				{
					// A nested type key
					RF_ASSERT( typeInfo.mValueType == reflect::Value::Type::Invalid );
					RF_ASSERT( typeInfo.mAccessor == nullptr );

					// Note the type for debug purposes
					details::Property_TryAddDebugTypeIDAttribute(
						exporter,
						params,
						*typeInfo.mClassInfo,
						location );

					// Recurse into it and expect it'll trigger an indentation
					shouldRecurse = true;
				}
				else if( typeInfo.mAccessor != nullptr )
				{
					// An accessor key
					RF_ASSERT( typeInfo.mValueType == reflect::Value::Type::Invalid );
					RF_ASSERT( typeInfo.mClassInfo == nullptr );

					RF_TODO_ANNOTATION( "Information about what this is?" );

					// Recurse into it and expect it'll trigger an indentation
					shouldRecurse = true;
				}
				else
				{
					RF_DBGFAIL_MSG( "Unexpected codepath" );
				}
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::AccessorTarget:
			{
				bool const isIndirection = varInst.mIndirectionInfo != nullptr;
				bool const isPolymorphic = isIndirection && details::IsPolymorphicIndirection( *varInst.mIndirectionInfo );

				char const* const name = "T"; // Target
				reflect::VariableTypeInfo const& typeInfo = varInst.mVariableTypeInfo;
				void const* const location = varInst.mVariableLocation;
				exporter.Instance_BeginNewProperty();
				exporter.Property_AddNameAttribute( name );
				if( typeInfo.mValueType != reflect::Value::Type::Invalid )
				{
					// A value target
					RF_ASSERT( typeInfo.mClassInfo == nullptr );
					RF_ASSERT( typeInfo.mAccessor == nullptr );

					// May or may not be an indirection, but certainly
					//  shouldn't be polymorphic
					RF_ASSERT( isPolymorphic == false );

					// Will serialize in-place, regardless of indirection,
					//  since it's simple to do so here, and won't require
					//  further traversal
					( (void)isIndirection );

					// Serialize immediately
					reflect::Value::Type const& type = typeInfo.mValueType;
					exporter.Property_AddValueAttribute( reflect::Value( type, location ) );
				}
				else if( typeInfo.mClassInfo != nullptr )
				{
					// A nested type target
					RF_ASSERT( typeInfo.mValueType == reflect::Value::Type::Invalid );
					RF_ASSERT( typeInfo.mAccessor == nullptr );

					// Note the type for debug purposes
					// NOTE: If polymorphic, this might be the instance type,
					//  not the pointer type, but that behavior may vary
					//  depending on the accessor, and the indirection info is
					//  the more correct way to check for that
					details::Property_TryAddDebugTypeIDAttribute(
						exporter,
						params,
						*typeInfo.mClassInfo,
						location );

					// Indirections may cause processing to be deferred
					bool shouldDefer = false;
					if( isIndirection )
					{
						if( params.mCollapseAllPossibleIndirections )
						{
							// Tentatively won't defer, unless we have to
							shouldDefer = false;
						}
						else
						{
							shouldDefer = true;
						}
					}
					if( isPolymorphic )
					{
						// Polymorphism should only happen through indirection,
						//  and in the current design that requires deferral,
						//  since type-ID is noted at the instance level, and
						//  not emitted at the property level
						RF_ASSERT( isIndirection );
						shouldDefer = true;
					}

					if( shouldDefer )
					{
						// Defer processing

						// Generate a new instance ID
						if( params.mInstanceIDGenerator == nullptr )
						{
							RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
								"Needed to defer an indirection, but was given"
								" no way to generate instance IDs" );
							RF_DBGFAIL();
							success = false;
							return;
						}
						exporter::InstanceID const deferredID = params.mInstanceIDGenerator();
						RF_ASSERT( deferredID != exporter::kInvalidInstanceID );

						// HACK: Setup a local indirection
						RF_TODO_ANNOTATION(
							"Some kind of mechanism to allow an external"
							" indirection, controlled by the caller" );
						// NOTE: Using altered ID for indirection and instance,
						//  to catch bugs
						// HACK: Unsafe indirection ID generation
						RF_TODO_ANNOTATION( "Safer indirection ID generation" )
						RF_ASSERT( deferredID < 10'000 );
						exporter::IndirectionID const indirectionID = deferredID + 10'000;
						RF_ASSERT( indirectionID != exporter::kInvalidIndirectionID );
						exporter.Root_RegisterLocalIndirection(
							indirectionID,
							deferredID );

						// Note the indirection ID on the property, to form one
						//  side of the link for later deserializers to use
						exporter.Property_AddIndirectionAttribute(
							indirectionID );

						// Report the deferred indirection so it can be
						//  processed during later logic
						if( params.mIndirectionDeferFunc == nullptr )
						{
							RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION,
								"Needed to defer an indirection, but the"
								" caller provided no callback to be notified"
								" of this" );
						}
						else
						{
							DeferredIndirection deferredIndirection = {};
							deferredIndirection.mInstanceID = deferredID;
							deferredIndirection.mVariableTypeInfo = varInst.mVariableTypeInfo;
							deferredIndirection.mVariableLocation = varInst.mVariableLocation;
							deferredIndirection.mIndirectionInfo = *varInst.mIndirectionInfo;
							params.mIndirectionDeferFunc( deferredIndirection );
						}
						shouldRecurse = false;
					}
					else
					{
						// Recurse into it and expect it'll trigger an indentation
						shouldRecurse = true;
					}
				}
				else if( typeInfo.mAccessor != nullptr )
				{
					// An accessor target
					RF_ASSERT( typeInfo.mValueType == reflect::Value::Type::Invalid );
					RF_ASSERT( typeInfo.mClassInfo == nullptr );

					// May or may not be an indirection, but current assumption
					//  is that accessors themselves can't be polymorphic, they
					//  can only at most have polymorphic targets within them
					RF_ASSERT( isPolymorphic == false );

					// Will serialize in-place, regardless of indirection,
					//  since at time of writing the primary reason to defer
					//  would be because of polymorphism, which it is expected
					//  that accessors cannot do
					( (void)isIndirection );

					RF_TODO_ANNOTATION( "Information about what this is?" );

					// Recurse into it and expect it'll trigger an indentation
					shouldRecurse = true;
				}
				else
				{
					RF_DBGFAIL_MSG( "Unexpected codepath" );
				}
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::Invalid:
			default:
				RF_DBGFAIL_MSG( "Unknown traversal type" );
				shouldRecurse = false;
				success = false;
				break;
		}
	};

	auto onReturnFromTraversalType =
		[&exporter, &success](
			rftype::TypeTraverser::TraversalType traversalType,
			rftype::TypeTraverser::TraversalVariableInstance const& varInst )
		-> void
	{
		switch( traversalType )
		{
			case RF::rftype::TypeTraverser::TraversalType::NestedType:
			{
				exporter.Property_OutdentFromLastIndent();
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::Accessor:
			{
				exporter.Property_OutdentFromLastIndent();
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::AccessorKey:
			{
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::AccessorTarget:
			{
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::Invalid:
			default:
				RF_DBGFAIL_MSG( "Unknown traversal type" );
				success = false;
				break;
		}
	};

	rftype::TypeTraverser::TraverseVariablesT( classInfo, classInstance, onMemberVariable, onTraversalTypeFound, onReturnFromTraversalType );

	return success;
}



bool ObjectSerializer::SerializeMultipleObjects(
	Exporter& exporter,
	reflect::ClassInfo const& classInfo,
	void const* classInstance,
	Params const& params )
{
	// Will wrap the params to make modifications as needed
	Params wrappedParams = params;

	// Root instance ID is optional, but if missing, it means that it won't be
	//  able to be referred to, which might create issues when trying to create
	//  external indirections (as opposed to just local local indirections)
	if( wrappedParams.mInstanceID == exporter::kInvalidInstanceID )
	{
		RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION,
			"Attempting to serialize multiple objects but instance ID for the"
			" root instance, which is a little suspicious" );
	}

	// This is a suspicious call, and might be better to be an error
	RF_TODO_ANNOTATION( "Re-evaluate whether this should be an error instead" );
	if( wrappedParams.mInstanceIDGenerator == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION,
			"Attempting to serialize multiple objects but without a way to"
			" generate new instance instance IDs, so will fail if multiple"
			" objects are actually needed" );
	}

	// May pick up more objects while serializing, these need to be tracked
	rftl::deque<DeferredIndirection> intermediates = {};
	{
		// Want to make sure we still call the original callback, if provided
		Params::IndirectionDeferFunc const& original = params.mIndirectionDeferFunc;

		auto const onDeferredIndirection =
			[&intermediates, &original](
				DeferredIndirection const& deferred )
			-> void
		{
			intermediates.emplace_back( deferred );
			if( original != nullptr )
			{
				original( deferred );
			}
		};

		wrappedParams.mIndirectionDeferFunc = onDeferredIndirection;
	}

	// Will track visited locations, to avoid duplicates or loops in complex
	//  pointer graphcs
	using VisitedList = rftl::unordered_map<void const*, reflect::VariableTypeInfo>;
	VisitedList visitedLocations;

	// Root instance
	{
		bool const rootSuccess =
			SerializeSingleObject(
				exporter,
				classInfo,
				classInstance,
				wrappedParams );
		if( rootSuccess == false )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
				"Failed to serialize root instance when attempting to serialize"
				" multiple objects" );
			return false;
		}
	}

	// Mark visited
	{
		reflect::VariableTypeInfo typeInfo = {};
		typeInfo.mClassInfo = &classInfo;
		bool const newInsert = visitedLocations.emplace( classInstance, rftl::move( typeInfo ) ).second;
		RF_ASSERT( newInsert );
	}


	// Intermediates
	while( intermediates.empty() == false )
	{
		using Node = DeferredIndirection;

		// Pop
		Node const currentNode = intermediates.front();
		intermediates.pop_front();

		// Location
		void const* const instanceLocation = currentNode.mVariableLocation;
		RF_ASSERT( instanceLocation != nullptr );

		// Type
		// HACK: Numerous assumptions based on current support in
		//  serializer code
		// TODO: Wider support
		RF_ASSERT( currentNode.mIndirectionInfo.mMemoryLookupInstanceType.has_value() );
		reflect::VariableTypeInfo const& instanceType = currentNode.mIndirectionInfo.mMemoryLookupInstanceType.value();
		RF_ASSERT( instanceType.mClassInfo != nullptr );
		reflect::ClassInfo const& instanceClassInfo = *instanceType.mClassInfo;

		// Check if visited
		VisitedList::const_iterator const visitedIter = visitedLocations.find( instanceLocation );
		if( visitedIter != visitedLocations.end() )
		{
			// Already emitted, skip
			RF_ASSERT_MSG( visitedIter->second == instanceType, "Type differs, corrupt?" );
			continue;
		}

		// Mark visited
		{
			bool const newInsert = visitedLocations.emplace( instanceLocation, instanceType ).second;
			RF_ASSERT( newInsert );
		}

		// Adjusted params
		Params instanceParams = wrappedParams;
		instanceParams.mInstanceID = currentNode.mInstanceID;

		// Intermediate
		bool const intermediateSuccess =
			SerializeSingleObject(
				exporter,
				instanceClassInfo,
				instanceLocation,
				instanceParams );
		if( intermediateSuccess == false )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
				"Failed to serialize intermediate instance when attempting to"
				" serialize multiple objects" );
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
