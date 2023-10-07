#include "stdafx.h"
#include "ObjectSerializer.h"

#include "Serialization/Exporter.h"

#include "Logging/Logging.h"

#include "core_rftype/StoredClassKey.h"
#include "core_rftype/TypeTraverser.h"


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

	return SerializeSingleObject(
		exporter,
		classInfo,
		classInstance,
		params );
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

	// Instance ID
	if( params.mInstanceID != exporter::kInvalidInstanceID )
	{
		exporter.Instance_AddInstanceIDAttribute( params.mInstanceID );
	}

	// Type ID
	details::Instance_TryAddTypeIDAttribute( exporter, params, classInfo, classInstance );

	auto onMemberVariable =
		[&exporter](
			rftype::TypeTraverser::MemberVariableInstance const& memberVariableInstance )
		-> void
	{
		char const* const name = memberVariableInstance.mMemberVariableInfo.mIdentifier;
		reflect::VariableTypeInfo const& typeInfo = memberVariableInstance.mMemberVariableInfo.mVariableTypeInfo;
		reflect::Value::Type const type = typeInfo.mValueType;
		void const* const location = memberVariableInstance.mMemberVariableLocation;
		exporter.Instance_BeginNewProperty();
		exporter.Property_AddNameAttribute( name );
		if( type != reflect::Value::Type::Invalid )
		{
			RF_ASSERT( typeInfo.mClassInfo == nullptr );
			RF_ASSERT( typeInfo.mAccessor == nullptr );
			exporter.Property_AddValueAttribute( reflect::Value( type, location ) );
		}
		else
		{
			// Probably a nested type or another accessor, expect a traversal
			//  callback soon
			RF_TODO_ANNOTATION( "Information about what this is?" );
		}
	};

	auto onTraversalTypeFound =
		[&exporter, &success](
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
				exporter.Property_IndentFromCurrentProperty();
				shouldRecurse = true;
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::Accessor:
			{
				// onMemberVariable(...) should've already prepared this node

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
				char const* const keyName = "K"; // Key
				reflect::VariableTypeInfo const& typeInfo = varInst.mVariableTypeInfo;
				reflect::Value::Type const type = typeInfo.mValueType;
				void const* const location = varInst.mVariableLocation;
				exporter.Instance_BeginNewProperty();
				exporter.Property_AddNameAttribute( keyName );
				if( type != reflect::Value::Type::Invalid )
				{
					RF_ASSERT( typeInfo.mClassInfo == nullptr );
					RF_ASSERT( typeInfo.mAccessor == nullptr );
					exporter.Property_AddValueAttribute( reflect::Value( type, location ) );
				}
				else
				{
					// Probably a nested type or another accessor, recurse into
					//  it and expect it'll trigger an indentation
					RF_TODO_ANNOTATION( "Information about what this is?" );
					shouldRecurse = true;
				}
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::AccessorTarget:
			{
				char const* const name = "T"; // Target
				reflect::VariableTypeInfo const& typeInfo = varInst.mVariableTypeInfo;
				reflect::Value::Type const type = typeInfo.mValueType;
				void const* const location = varInst.mVariableLocation;
				exporter.Instance_BeginNewProperty();
				exporter.Property_AddNameAttribute( name );
				if( type != reflect::Value::Type::Invalid )
				{
					RF_ASSERT( typeInfo.mClassInfo == nullptr );
					RF_ASSERT( typeInfo.mAccessor == nullptr );
					exporter.Property_AddValueAttribute( reflect::Value( type, location ) );
				}
				else
				{
					// Probably a nested type or another accessor, recurse into
					//  it and expect it'll trigger an indentation
					RF_TODO_ANNOTATION( "Information about what this is?" );
					shouldRecurse = true;
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

///////////////////////////////////////////////////////////////////////////////
}
