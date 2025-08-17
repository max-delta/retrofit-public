#include "stdafx.h"
#include "XmlExporter.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"
#include "core_reflect/Value.h"

#include "rftl/cstring"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
static bool hasAttribute( pugi::xml_node const& node, char const* attributeName )
{
	for( pugi::xml_attribute const& attribute : node.attributes() )
	{
		if( rftl::string_view( attribute.name() ) == rftl::string_view( attributeName ) )
		{
			return true;
		}
	}
	return false;
}
#endif



static void AppendDebugTypeDataIfNotADuplicate( pugi::xml_node& debugData, Exporter::TypeID const& typeID, char const* debugName )
{
	for( pugi::xml_node const& node : debugData.children() )
	{
		RF_TODO_ANNOTATION( "More robust error-handling and generally less shitty code" );
		RF_ASSERT( node.name() == rftl::string_view( "TypeID" ) );
		RF_ASSERT( hasAttribute( node, "Value" ) );
		RF_ASSERT( hasAttribute( node, "DebugName" ) );
		bool const valueMatch =
			node.find_attribute(
					[]( pugi::xml_attribute const& attribute ) -> bool
					{
						return attribute.name() == rftl::string_view( "Value" );
					} )
				.as_ullong() == typeID;
		bool const nameMatch =
			node.find_attribute(
					[]( pugi::xml_attribute const& attribute ) -> bool
					{
						return attribute.name() == rftl::string_view( "DebugName" );
					} )
				.as_string() == rftl::string_view( debugName );
		if( valueMatch && nameMatch )
		{
			// Duplicate
			return;
		}
	}

	// Not a duplicate, add it
	pugi::xml_node typeIDNode = debugData.append_child( "TypeID" );
	typeIDNode.append_attribute( "Value" ) = typeID;
	typeIDNode.append_attribute( "DebugName" ) = debugName;
}

}
///////////////////////////////////////////////////////////////////////////////

XmlExporter::XmlExporter()
{
	// Major sections
	mHeader = mDoc.append_child( "Header" );
	mTableOfContents = mDoc.append_child( "TableOfContents" );
	mExternalDependencies = mDoc.append_child( "ExternalDependencies" );
	mInternalDependencies = mDoc.append_child( "InternalDependencies" );
	mData = mDoc.append_child( "Data" );
	mDebugData = mDoc.append_child( "DebugData" );

	mHeader.append_attribute( "Magic" ) = "RetroFit Resource";
	mHeader.append_attribute( "MajorVersion" ) = 0;
	mHeader.append_attribute( "MinorVersion" ) = 1;
	mHeader.append_attribute( "SubminorVersion" ) = 0;
}



XmlExporter::~XmlExporter()
{
	RFLOG_TEST_AND_NOTIFY( mFinalized, nullptr, RFCAT_SERIALIZATION, "Exporter destructed without finalizing" );
}



bool XmlExporter::WriteToFile( FILE* file ) const
{
	if( mFinalized == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Exporter attempting to write before being finalized" );
		return false;
	}

	pugi::xml_writer_file writer = pugi::xml_writer_file( file );
	mDoc.save( writer );
	return true;
}



bool XmlExporter::WriteToString( rftl::string& string ) const
{
	if( mFinalized == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Exporter attempting to write before being finalized" );
		return false;
	}

	class xml_writer_string : public pugi::xml_writer
	{
		RF_NO_COPY( xml_writer_string );

	public:
		xml_writer_string( rftl::string& string )
			: mString( string )
		{
			//
		}

		virtual void write( void const* data, size_t size ) override
		{
			mString.append( reinterpret_cast<char const*>( data ), size );
		}

		rftl::string& mString;
	};

	xml_writer_string writer{ string };
	mDoc.save( writer );
	return true;
}



bool XmlExporter::Root_FinalizeExport()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter attempting to re-finalize" );
		return false;
	}

	mHeader = {};
	mTableOfContents = {};
	mExternalDependencies = {};
	mInternalDependencies = {};
	mData = {};
	mDebugData = {};

	mCurrentInstance = {};
	mCurrentInstanceID = exporter::kInvalidInstanceID;
	mTOCInstances = {};
	mPropertyStack.clear();

	mFinalized = true;
	return true;
}



bool XmlExporter::Root_BeginNewInstance()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	mCurrentInstance = mData.append_child( "Instance" );
	mCurrentInstanceID = exporter::kInvalidInstanceID;
	mPropertyStack.clear();
	mNewIndent = false;
	return true;
}



bool XmlExporter::Root_RegisterLocalIndirection( IndirectionID const& indirectionID, InstanceID const& instanceID )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	pugi::xml_node dependency = mInternalDependencies.append_child( "Dependency" );
	dependency.append_attribute( "IndirectionID" ) = indirectionID;
	dependency.append_attribute( "InstanceID" ) = instanceID;
	return true;
}



bool XmlExporter::Root_RegisterExternalIndirection( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	pugi::xml_node dependency = mExternalDependencies.append_child( "Dependency" );
	dependency.append_attribute( "IndirectionID" ) = indirectionID;
	dependency.append_attribute( "ExternalReferenceID" ) = referenceID;
	return true;
}



bool XmlExporter::Instance_AddInstanceIDAttribute( InstanceID const& instanceID )
{
	if( instanceID == exporter::kInvalidInstanceID )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Exporter receiving an invalid instance ID" );
		return false;
	}

	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	mCurrentInstance.append_attribute( "ID" ) = instanceID;
	mCurrentInstanceID = instanceID;

	if( mTOCInstances.count( instanceID ) > 0 )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Table of contents received a duplicate instance ID" );
		return false;
	}
	pugi::xml_node tocEntry = mTableOfContents.append_child( "Instance" );
	tocEntry.append_attribute( "ID" ) = instanceID;
	bool const newEntry = mTOCInstances.emplace( instanceID, tocEntry ).second;
	RF_ASSERT( newEntry );

	return true;
}



bool XmlExporter::Instance_AddTypeIDAttribute( TypeID const& typeID, char const* debugName )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	// NOTE: Adding debug name first, so it appears before the type ID, since
	//  this is purely for human-readable diagnostics
	if( mConfig.mIncludeInlineDebugNames )
	{
		RF_ASSERT( details::hasAttribute( mCurrentInstance, "DebugName" ) == false );
		mCurrentInstance.append_attribute( "DebugName" ) = debugName;
	}

	RF_ASSERT( details::hasAttribute( mCurrentInstance, "TypeID" ) == false );
	mCurrentInstance.append_attribute( "TypeID" ) = typeID;

	if( mConfig.mStripDebugDataSection == false )
	{
		details::AppendDebugTypeDataIfNotADuplicate( mDebugData, typeID, debugName );
	}

	// Want to update the table of contents with this type info
	if( mCurrentInstanceID == exporter::kInvalidInstanceID )
	{
		RF_TODO_BREAK_MSG(
			"Type ID without instance ID, need to defer TOC update and wait"
			" to see if an instance ID comes in, and then add the TOC entry"
			" with the tpye info during the instance ID logic" );
	}
	else
	{
		// Instance ID logic should've made this entry for us
		pugi::xml_node& tocEntry = mTOCInstances.at( mCurrentInstanceID );
		tocEntry.append_attribute( "TypeID" ) = typeID;
	}

	return true;
}



bool XmlExporter::Instance_BeginNewProperty()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mPropertyStack.empty() )
	{
		// No properties yet, create a new property on the instance
		RF_ASSERT( mNewIndent == false );
		mPropertyStack.emplace_back( mCurrentInstance.append_child( "Property" ) );
	}
	else if( mNewIndent )
	{
		// Will use the property that was made from the last indent
		mNewIndent = false;
	}
	else
	{
		// Have an existing property, need to close it and make a new one
		RF_ASSERT( mPropertyStack.empty() == false );
		RF_ASSERT( mNewIndent == false );

		// Close existing
		mPropertyStack.pop_back();

		if( mPropertyStack.empty() )
		{
			// That was the only property, need to make another at stack bottom
			mPropertyStack.emplace_back( mCurrentInstance.append_child( "Property" ) );
		}
		else
		{
			// Nest a new one to keep our previous stack depth
			pugi::xml_node& currentProperty = mPropertyStack.back();
			mPropertyStack.emplace_back( currentProperty.append_child( "Property" ) );
		}
	}

	return true;
}



bool XmlExporter::Property_AddNameAttribute( char const* name )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	pugi::xml_node& currentProperty = mPropertyStack.back();
	RF_ASSERT( details::hasAttribute( currentProperty, "Name" ) == false );
	currentProperty.append_attribute( "Name" ) = name;
	return true;
}



bool XmlExporter::Property_AddValueAttribute( reflect::Value const& value )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	pugi::xml_node& currentProperty = mPropertyStack.back();
	RF_ASSERT( details::hasAttribute( currentProperty, "Type" ) == false );
	RF_ASSERT( details::hasAttribute( currentProperty, "Value" ) == false );
	pugi::xml_attribute typeAttr = currentProperty.append_attribute( "Type" );
	pugi::xml_attribute valueAttr = currentProperty.append_attribute( "Value" );

	reflect::Value::Type const type = value.GetStoredType();
	{
		char const* typeName;
		if( type != reflect::Value::Type::Invalid )
		{
			typeName = reflect::Value::GetTypeName( type );
		}
		else
		{
			typeName = "INVALID";
		}
		typeAttr = typeName;
	}

	switch( type )
	{

#define RF_CASE( TYPE, VAL ) \
	case TYPE: \
		valueAttr = VAL; \
		break

#define RF_CASE_UNSUPPORTED( TYPE, VAL ) \
	case TYPE: \
		RFLOGF_WARNING( nullptr, RFCAT_SERIALIZATION, "Unsupported type '{}' will be forced to a predetermined value", value.GetStoredTypeName() ); \
		valueAttr = VAL; \
		break

		RF_CASE( reflect::Value::Type::Bool, ( *value.GetAs<bool>() ) ? 1u : 0u );
		RF_CASE_UNSUPPORTED( reflect::Value::Type::VoidPtr, "<PTR>" );
		RF_CASE_UNSUPPORTED( reflect::Value::Type::VoidConstPtr, "<PTR>" );
		RF_CASE_UNSUPPORTED( reflect::Value::Type::VirtualClassPtr, "<PTR>" );
		RF_CASE_UNSUPPORTED( reflect::Value::Type::VirtualClassConstPtr, "<PTR>" );
		RF_CASE( reflect::Value::Type::Char, *value.GetAs<char>() );
		RF_CASE( reflect::Value::Type::WChar, *value.GetAs<wchar_t>() );
		RF_CASE( reflect::Value::Type::Char8, *value.GetAs<char8_t>() );
		RF_CASE( reflect::Value::Type::Char16, *value.GetAs<char16_t>() );
		RF_CASE( reflect::Value::Type::Char32, *value.GetAs<char32_t>() );
		RF_CASE( reflect::Value::Type::Float, *value.GetAs<float>() );
		RF_CASE( reflect::Value::Type::Double, *value.GetAs<double>() );
		RF_CASE( reflect::Value::Type::LongDouble, math::float_cast<double>( *value.GetAs<long double>() ) );
		RF_CASE( reflect::Value::Type::UInt8, *value.GetAs<uint8_t>() );
		RF_CASE( reflect::Value::Type::UInt16, *value.GetAs<uint16_t>() );
		RF_CASE( reflect::Value::Type::UInt32, *value.GetAs<uint32_t>() );
		RF_CASE( reflect::Value::Type::UInt64, *value.GetAs<uint64_t>() );
		RF_CASE( reflect::Value::Type::Int8, *value.GetAs<int8_t>() );
		RF_CASE( reflect::Value::Type::Int16, *value.GetAs<int16_t>() );
		RF_CASE( reflect::Value::Type::Int32, *value.GetAs<int32_t>() );
		RF_CASE( reflect::Value::Type::Int64, *value.GetAs<int64_t>() );

#undef RF_CASE_UNSUPPORTED
#undef RF_CASE

		case reflect::Value::Type::Invalid:
		default:
			valueAttr = "<UNSUPPORTED>";
			break;
	}
	return true;
}



bool XmlExporter::Property_AddDebugTypeIDAttribute( TypeID const& debugTypeID, char const* debugName )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mConfig.mIncludeDebugTypeIDs == false )
	{
		// Not including these
		return true;
	}

	pugi::xml_node& currentProperty = mPropertyStack.back();

	// NOTE: Adding debug name first, so it appears before the type ID, since
	//  this is purely for human-readable diagnostics
	if( mConfig.mIncludeInlineDebugNames )
	{
		RF_ASSERT( details::hasAttribute( currentProperty, "DebugName" ) == false );
		currentProperty.append_attribute( "DebugName" ) = debugName;
	}

	RF_ASSERT( details::hasAttribute( currentProperty, "DebugTypeID" ) == false );
	currentProperty.append_attribute( "DebugTypeID" ) = debugTypeID;

	if( mConfig.mStripDebugDataSection == false )
	{
		details::AppendDebugTypeDataIfNotADuplicate( mDebugData, debugTypeID, debugName );
	}

	return true;
}



bool XmlExporter::Property_AddIndirectionAttribute( IndirectionID const& indirectionID )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	pugi::xml_node& currentProperty = mPropertyStack.back();
	currentProperty.append_attribute( "IndirectionID" ) = indirectionID;
	return true;
}



bool XmlExporter::Property_IndentFromCurrentProperty()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	pugi::xml_node& currentProperty = mPropertyStack.back();
	mPropertyStack.emplace_back( currentProperty.append_child( "Property" ) );
	RF_ASSERT( mNewIndent == false );
	mNewIndent = true;
	return true;
}



bool XmlExporter::Property_OutdentFromLastIndent()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	// NOTE: While an empty indent is probably a sign of a bug, it is possible
	//  to immediately outdent right after an indent
	if( mNewIndent )
	{
		RF_DBGFAIL_MSG( "Outdent after indent, empty property?" );
		mNewIndent = false;
	}

	mPropertyStack.pop_back();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
