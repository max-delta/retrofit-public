#include "stdafx.h"
#include "ObjectDeserializer.h"

#include "Logging/Logging.h"
#include "Serialization/Importer.h"

#include "core_math/math_clamps.h"
#include "core_rftype/TypeTraverser.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

using VariableIdentifier = rftl::vector<rftl::string>;



struct SingleScratchSpace
{
	RF_NO_COPY( SingleScratchSpace );

	SingleScratchSpace(
		reflect::ClassInfo const& classInfo,
		void* classInstance )
		: classInfo( classInfo )
		, classInstance( classInstance )
	{
		//
	}

	reflect::ClassInfo const& classInfo;
	void* const classInstance;

	size_t instanceCount = 0;
	VariableIdentifier propertyStack;
};

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
		scratch.instanceCount++;
		if( scratch.instanceCount > 1 )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
				"Multiple instances encountered during single object"
				" deserialization, these will be skipped" );
			return false;
		}

		scratch.propertyStack.clear();
		scratch.propertyStack.emplace_back( "<this>" );

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
		RF_ASSERT( scratch.instanceCount == 1 );

		// Don't care, only support one object
		return true;
	};


	callbacks.mInstance_AddTypeIDAttributeFunc =
		[&scratch]( TypeID const& typeID, char const* debugName ) -> bool
	{
		RF_ASSERT( scratch.instanceCount == 1 );

		RF_TODO_ANNOTATION( "Make sure the type ID matches what we're expecting" );
		return true;
	};


	callbacks.mInstance_BeginNewPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.instanceCount == 1 );

		RF_ASSERT( scratch.propertyStack.size() >= 1 );
		scratch.propertyStack.back() = "<UNSET>";

		return true;
	};


	callbacks.mProperty_AddNameAttributeFunc =
		[&scratch]( rftl::string_view const& name ) -> bool
	{
		RF_ASSERT( scratch.instanceCount == 1 );

		RF_ASSERT( scratch.propertyStack.size() >= 1 );
		scratch.propertyStack.back() = name;

		return true;
	};


	callbacks.mProperty_AddValueAttributeFunc =
		[&scratch]( reflect::Value const& value ) -> bool
	{
		RF_ASSERT( scratch.instanceCount == 1 );

		RFLOG_WARNING( scratch.propertyStack, RFCAT_SERIALIZATION, "TODO: Set value" );
		RF_TODO_BREAK_MSG( "Use some fancy path-based helper to set this" );

		return true;
	};


	callbacks.mProperty_AddIndirectionAttributeFunc =
		[&scratch]( IndirectionID const& indirectionID ) -> bool
	{
		RF_ASSERT( scratch.instanceCount == 1 );

		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION,
			"Indirections not supported in single object deserialization" );
		return false;
	};


	callbacks.mProperty_IndentFromCurrentPropertyFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.instanceCount == 1 );

		RF_ASSERT( scratch.propertyStack.size() >= 1 );
		scratch.propertyStack.emplace_back( "<UNSET>" );

		return true;
	};


	callbacks.mProperty_OutdentFromLastIndentFunc =
		[&scratch]() -> bool
	{
		RF_ASSERT( scratch.instanceCount == 1 );

		RF_ASSERT( scratch.propertyStack.size() >= 1 );
		scratch.propertyStack.pop_back();

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
static void RF::logging::WriteContextString( RF::serialization::details::VariableIdentifier const& context, Utf8LogContextBuffer& buffer )
{
	size_t bufferOffset = 0;
	size_t const maxBufferOffset = buffer.size();

	size_t const numElements = context.size();
	for( size_t i = 0; i < numElements; i++ )
	{
		RF::serialization::details::VariableIdentifier::value_type const& element = context.at( i );

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

		size_t const bytesRemaining = maxBufferOffset - bufferOffset;
		size_t const bytesToWrite = math::Min( bytesRemaining, element.size() );
		memcpy( &buffer[bufferOffset], element.data(), bytesToWrite );
		bufferOffset += bytesToWrite;
	}
}
