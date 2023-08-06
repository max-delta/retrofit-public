#include "stdafx.h"
#include "ObjectSerializer.h"

#include "Serialization/Exporter.h"

#include "core_rftype/TypeTraverser.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

bool ObjectSerializer::SerializeSingleObject( Exporter& exporter, reflect::ClassInfo const& classInfo, void const* classInstance )
{
	bool success;

	success = exporter.Root_BeginNewInstance();
	if( success == false )
	{
		return false;
	}

	// Not writing an instance ID, since there's only one and can't have
	//  indirect references made to it
	//exporter.Instance_AddInstanceIDAttribute( ... );

	// Not writing a type ID, since that requires more complicated machinery
	//  that we don't want to invoke so we can keep the single object
	//  serialization simple
	// NOTE: This means deserialization will need to have the root type made
	//  for it, so single object serialization will need some other way to know
	//  what type was serialized, such as filename extension or packet headers
	//exporter.Instance_AddTypeIDAttribute( ... );

	auto onMemberVariable = [&exporter](
		rftype::TypeTraverser::MemberVariableInstance const& memberVariableInstance ) ->
		void
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

	auto onTraversalTypeFound = [&exporter, &success](
		rftype::TypeTraverser::TraversalType traversalType,
		rftype::TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
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
				exporter.Property_IndentFromCurrentProperty();
				shouldRecurse = true;
				break;
			}
			case RF::rftype::TypeTraverser::TraversalType::AccessorKey:
			{
				char const* const name = "K"; // Key
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

	auto onReturnFromTraversalType = [&exporter, &success](
		rftype::TypeTraverser::TraversalType traversalType,
		rftype::TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
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
