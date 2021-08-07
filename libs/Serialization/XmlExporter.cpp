#include "stdafx.h"
#include "XmlExporter.h"

#include "core_math/math_casts.h"

#include "rftl/cstring"


namespace RF::serialization {
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
	//
}



bool XmlExporter::WriteToFile( FILE* file ) const
{
	pugi::xml_writer_file writer = pugi::xml_writer_file( file );
	mDoc.save( writer );
	return true;
}



bool XmlExporter::WriteToString( rftl::string& string ) const
{
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
	mHeader = {};
	mTableOfContents = {};
	mExternalDependencies = {};
	mInternalDependencies = {};
	mData = {};
	mDebugData = {};

	mCurrentInstance = {};
	mPropertyStack.clear();
	return true;
}



bool XmlExporter::Root_BeginNewInstance()
{
	mCurrentInstance = mData.append_child( "Instance" );
	mPropertyStack.clear();
	mNewIndent = false;
	return true;
}



bool XmlExporter::Root_RegisterLocalIndirection( IndirectionID const& indirectionID, InstanceID const& instanceID )
{
	pugi::xml_node dependency = mInternalDependencies.append_child( "Dependency" );
	dependency.append_attribute( "IndirectionID" ) = indirectionID;
	dependency.append_attribute( "InstanceID" ) = instanceID;
	return true;
}



bool XmlExporter::Root_RegisterExternalIndirection( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID )
{
	pugi::xml_node dependency = mExternalDependencies.append_child( "Dependency" );
	dependency.append_attribute( "IndirectionID" ) = indirectionID;
	dependency.append_attribute( "ExternalReferenceID" ) = referenceID;
	return true;
}



bool XmlExporter::Instance_AddInstanceIDAttribute( InstanceID const& instanceID )
{
	mCurrentInstance.append_attribute( "ID" ) = instanceID;
	mTableOfContents.append_child( "Instance" ).append_attribute( "ID" ) = instanceID;
	return true;
}



bool XmlExporter::Instance_AddTypeIDAttribute( TypeID const& typeID, char const* debugName )
{
	mCurrentInstance.append_attribute( "TypeID" ) = typeID;

	pugi::xml_node typeIDNode = mDebugData.append_child( "TypeID" );
	typeIDNode.append_attribute( "Value" ) = typeID;
	typeIDNode.append_attribute( "DebugName" ) = debugName;

	return true;
}



bool XmlExporter::Instance_BeginNewProperty()
{
	if( mPropertyStack.empty() )
	{
		RF_ASSERT( mNewIndent == false );
		mPropertyStack.emplace_back();
	}

	if( mNewIndent )
	{
		// Will use the property from the last indent
		mNewIndent = false;
	}
	else
	{
		pugi::xml_node& currentProperty = mPropertyStack.back();
		currentProperty = mCurrentInstance.append_child( "Property" );
	}
	return true;
}



bool XmlExporter::Property_AddNameAttribute( char const* name )
{
	pugi::xml_node& currentProperty = mPropertyStack.back();
	currentProperty.append_attribute( "Name" ) = name;
	return true;
}



bool XmlExporter::Property_AddValueAttribute( reflect::Value const& value )
{
	pugi::xml_node& currentProperty = mPropertyStack.back();
	pugi::xml_attribute typeAttr = currentProperty.append_attribute( "Type" );
	pugi::xml_attribute valueAttr = currentProperty.append_attribute( "Value" );

	reflect::Value::Type const type = value.GetStoredType();
	char const* const typeName = reflect::Value::GetTypeName( type );
	typeAttr = typeName;
	switch( type )
	{
		case reflect::Value::Type::Bool:				valueAttr = *value.GetAs<bool>(); break;
		case reflect::Value::Type::VoidPtr:				valueAttr = "<PTR>"; break;
		case reflect::Value::Type::VoidConstPtr:		valueAttr = "<PTR>"; break;
		case reflect::Value::Type::VirtualClassPtr:		valueAttr = "<PTR>"; break;
		case reflect::Value::Type::VirtualClassConstPtr:valueAttr = "<PTR>"; break;
		case reflect::Value::Type::Char:				valueAttr = *value.GetAs<char>(); break;
		case reflect::Value::Type::WChar:				valueAttr = *value.GetAs<wchar_t>(); break;
		case reflect::Value::Type::Char16:				valueAttr = *value.GetAs<char16_t>(); break;
		case reflect::Value::Type::Char32:				valueAttr = *value.GetAs<char32_t>(); break;
		case reflect::Value::Type::Float:				valueAttr = *value.GetAs<float>(); break;
		case reflect::Value::Type::Double:				valueAttr = *value.GetAs<double>(); break;
		case reflect::Value::Type::LongDouble:			valueAttr = math::float_cast<double>( *value.GetAs<long double>() ); break;
		case reflect::Value::Type::UInt8:				valueAttr = *value.GetAs<uint8_t>(); break;
		case reflect::Value::Type::UInt16:				valueAttr = *value.GetAs<uint16_t>(); break;
		case reflect::Value::Type::UInt32:				valueAttr = *value.GetAs<uint32_t>(); break;
		case reflect::Value::Type::UInt64:				valueAttr = *value.GetAs<uint64_t>(); break;
		case reflect::Value::Type::Int8:				valueAttr = *value.GetAs<int8_t>(); break;
		case reflect::Value::Type::Int16:				valueAttr = *value.GetAs<int16_t>(); break;
		case reflect::Value::Type::Int32:				valueAttr = *value.GetAs<int32_t>(); break;
		case reflect::Value::Type::Int64:				valueAttr = *value.GetAs<int64_t>(); break;
		case reflect::Value::Type::Invalid:
		default:
			valueAttr = "<UNSUPPORTED>"; break;
	}
	return true;
}



bool XmlExporter::Property_AddIndirectionAttribute( IndirectionID const& indirectionID )
{
	pugi::xml_node& currentProperty = mPropertyStack.back();
	currentProperty.append_attribute( "IndirectionID" ) = indirectionID;
	return true;
}



bool XmlExporter::Property_IndentFromCurrentProperty()
{
	pugi::xml_node& currentProperty = mPropertyStack.back();
	mPropertyStack.emplace_back( currentProperty.append_child( "Property" ) );
	RF_ASSERT( mNewIndent == false );
	mNewIndent = true;
	return true;
}



bool XmlExporter::Property_OutdentFromLastIndent()
{
	mPropertyStack.pop_back();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
