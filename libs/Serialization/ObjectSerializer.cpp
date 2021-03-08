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
		reflect::Value::Type const type = memberVariableInstance.mMemberVariableInfo.mVariableTypeInfo.mValueType;
		void const* const location = memberVariableInstance.mMemberVariableLocation;
		exporter.Instance_BeginNewProperty();
		exporter.Property_AddNameAttribute( name );
		exporter.Property_AddValueAttribute( reflect::Value( type, location ) );
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
			case RF::rftype::TypeTraverser::TraversalType::AccessorKey:
			case RF::rftype::TypeTraverser::TraversalType::AccessorTarget:
				RF_TODO_BREAK_MSG( "UPDATE TO HANDLE NEW TRAVERSAL LOGIC, AND TEST!!!" );
				shouldRecurse = false;
				success = false;
				break;
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
			case RF::rftype::TypeTraverser::TraversalType::AccessorKey:
			case RF::rftype::TypeTraverser::TraversalType::AccessorTarget:
				RF_TODO_BREAK_MSG( "UPDATE TO HANDLE NEW TRAVERSAL LOGIC, AND TEST!!!" );
				success = false;
				break;
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
