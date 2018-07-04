#include "stdafx.h"
#include "ObjectSerializer.h"

#include "Serialization/Exporter.h"

#include "core_rftype/TypeTraverser.h"


namespace RF { namespace serialization {
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

	auto onNestedTypeFound = [&exporter](
		rftype::TypeTraverser::MemberVariableInstance const& memberVariableInstance, bool& shouldRecurse ) ->
		void
	{
		char const* const name = memberVariableInstance.mMemberVariableInfo.mIdentifier;
		exporter.Instance_BeginNewProperty();
		exporter.Property_AddNameAttribute( name );
		exporter.Property_IndentFromCurrentProperty();
		shouldRecurse = true;
	};

	auto onReturnFromNestedType = [&exporter](
		void ) ->
		void
	{
		exporter.Property_OutdentFromLastIndent();
	};

	rftype::TypeTraverser::TraverseVariablesT( classInfo, classInstance, onMemberVariable, onNestedTypeFound, onReturnFromNestedType );

	success = exporter.Root_BeginNewInstance();
	if( success == false )
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}
