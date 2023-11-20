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
		classKey.mName.c_str() );
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
		classKey.mName.c_str() );
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

	Intermediates intermediates = {};
	bool const success = SerializeSingleObject(
		exporter,
		classInfo,
		classInstance,
		params,
		intermediates );

	// Should fail to be able to produce indirections, since we did not provide
	//  a way to generate the IDs for them
	// NOTE: Expect the serialize to have return as failed if it tried
	RF_ASSERT( intermediates.mDeferredIndirections.empty() );

	return success;
}



bool ObjectSerializer::SerializeSingleObject(
	Exporter& exporter,
	reflect::ClassInfo const& classInfo,
	void const* classInstance,
	Params const& params,
	Intermediates& intermediates )
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
		[&exporter, &params, &intermediates, &success](
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

						// Note the ID on the property, to form one side of the
						//  link for later deserializers to use
						// NOTE: At time of writing the indirection ID and the
						//  instance ID are the same ID
						exporter::IndirectionID const indirectionID = deferredID;
						RF_ASSERT( indirectionID != exporter::kInvalidIndirectionID );
						exporter.Property_AddIndirectionAttribute(
							indirectionID );

						// Add the deferred indirection to a list to process
						//  during later logic
						Intermediates::DeferredIndirection deferredIndirection = {};
						deferredIndirection.mInstanceID = deferredID;
						deferredIndirection.mVariableTypeInfo = varInst.mVariableTypeInfo;
						deferredIndirection.mVariableLocation = varInst.mVariableLocation;
						deferredIndirection.mIndirectionInfo = *varInst.mIndirectionInfo;
						intermediates.mDeferredIndirections.emplace_back( rftl::move( deferredIndirection ) );
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
	// Root instance ID is optional, but if missing, it means that it won't be
	//  able to be referred to, which might create issues when trying to create
	//  external indirections (as opposed to just local local indirections)
	if( params.mInstanceID == exporter::kInvalidInstanceID )
	{
		RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION,
			"Attempting to serialize multiple objects but instance ID for the"
			" root instance, which is a little suspicious" );
	}

	// This is a suspicious call, and might be better to be an error
	RF_TODO_ANNOTATION( "Re-evaluate whether this should be an error instead" );
	if( params.mInstanceIDGenerator == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION,
			"Attempting to serialize multiple objects but without a way to"
			" generate new instance instance IDs, so will fail if multiple"
			" objects are actually needed" );
	}

	// May pick up more objects while serializing
	Intermediates intermediates = {};

	// Will track visited locations, to avoid duplicates or loops in complex
	//  pointer graphcs
	using VisitedList = rftl::unordered_map<void const*, reflect::VariableTypeInfo>;
	VisitedList visitedLocations;

	// Root instance
	{
		bool const rootSuccess =
			serialization::ObjectSerializer::SerializeSingleObject(
				exporter,
				classInfo,
				classInstance,
				params,
				intermediates );
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
	while( intermediates.mDeferredIndirections.empty() == false )
	{
		using Node = serialization::ObjectSerializer::Intermediates::DeferredIndirection;

		// Pop
		Node const currentNode = intermediates.mDeferredIndirections.front();
		intermediates.mDeferredIndirections.pop_front();

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
		Params instanceParams = params;
		instanceParams.mInstanceID = currentNode.mInstanceID;

		// Intermediate
		bool const intermediateSuccess =
			serialization::ObjectSerializer::SerializeSingleObject(
				exporter,
				instanceClassInfo,
				instanceLocation,
				instanceParams,
				intermediates );
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
