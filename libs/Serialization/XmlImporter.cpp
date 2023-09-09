#include "stdafx.h"
#include "XmlImporter.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"
#include "core_reflect/Value.h"

RF_TODO_ANNOTATION( "Remove c_str usages, update logging to C++20 format" );
#include "rftl/extension/c_str.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/string_parse.h"
#include "rftl/optional"
#include "rftl/sstream"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// HACK: Can't be constexpr, since source values are non-constexpr
static unsigned int GetCommonParseMode()
{
	// Currently don't have a reason to enable any wierd XML features
	return pugi::parse_minimal;
}



template<typename FuncT, typename... ArgsT>
static bool TryInvoke( FuncT const& func, ArgsT const&... args )
{
	if( func == nullptr )
	{
		// Return means "keep processing", and null indicates caller doesn't
		//  care what happens, so returning true to keep going
		return true;
	}

	return func( args... );
}



template<typename ValT>
static bool TryParse( ValT& value, rftl::string_view const& str )
{
	rftl::stringstream ss;
	ss << str;
	ss >> value;

	if( ss.rdbuf()->in_avail() != 0 )
	{
		return false;
	}

	return true;
}



static reflect::Value TryConvertValue( rftl::string_view const& type, rftl::string_view const& value )
{
	if( value.empty() )
	{
		return {};
	}

#define RF_TEST( ENUM, VARTYPE ) \
	if( type == reflect::Value::GetTypeName( ENUM ) ) \
	{ \
		VARTYPE temp = {}; \
		bool const success = TryParse( temp, value ); \
		if( success == false ) \
		{ \
			return reflect::Value{}; \
		} \
		return reflect::Value( temp ); \
	}
#define RF_TEST_I( ENUM, INTERMEDIATE, VARTYPE ) \
	if( type == reflect::Value::GetTypeName( ENUM ) ) \
	{ \
		static_assert( rftl::is_integral<INTERMEDIATE>::value ); \
		static_assert( rftl::is_integral<VARTYPE>::value ); \
		static_assert( rftl::is_signed<INTERMEDIATE>::value == rftl::is_signed<VARTYPE>::value ); \
		static_assert( sizeof( INTERMEDIATE ) == sizeof( VARTYPE ) ); \
		INTERMEDIATE temp = {}; \
		bool const success = TryParse( temp, value ); \
		if( success == false ) \
		{ \
			return reflect::Value{}; \
		} \
		VARTYPE emit = math::integer_cast<VARTYPE>( temp ); \
		return reflect::Value( emit ); \
	}
#define RF_TEST_S( ENUM, VARTYPE, VALUE ) \
	if( type == reflect::Value::GetTypeName( ENUM ) ) \
	{ \
		VARTYPE temp = VALUE; \
		RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION, "Unsupported type '%s' will be forced to a predetermined value", RFTLE_CSTR( type ) ); \
		return reflect::Value( temp ); \
	}

	RF_TEST( reflect::Value::Type::Bool, bool );
	RF_TEST_S( reflect::Value::Type::VoidPtr, void*, nullptr );
	RF_TEST_S( reflect::Value::Type::VoidConstPtr, void const*, nullptr );
	RF_TEST_S( reflect::Value::Type::VirtualClassPtr, void const*, nullptr );
	RF_TEST_S( reflect::Value::Type::VirtualClassConstPtr, void const*, nullptr );
	RF_TEST( reflect::Value::Type::Char, char );
	RF_TEST_I( reflect::Value::Type::WChar, uint16_t, wchar_t );
	RF_TEST_I( reflect::Value::Type::Char16, uint16_t, char16_t );
	RF_TEST_I( reflect::Value::Type::Char32, uint32_t, char32_t );
	RF_TEST( reflect::Value::Type::Float, float );
	RF_TEST( reflect::Value::Type::Double, double );
	RF_TEST( reflect::Value::Type::LongDouble, long double );
	RF_TEST( reflect::Value::Type::UInt8, uint8_t );
	RF_TEST( reflect::Value::Type::UInt16, uint16_t );
	RF_TEST( reflect::Value::Type::UInt32, uint32_t );
	RF_TEST( reflect::Value::Type::UInt64, uint64_t );
	RF_TEST( reflect::Value::Type::Int8, int8_t );
	RF_TEST( reflect::Value::Type::Int16, int16_t );
	RF_TEST( reflect::Value::Type::Int32, int32_t );
	RF_TEST( reflect::Value::Type::Int64, int64_t );

#undef RF_TEST
#undef RF_TEST_I
#undef RF_TEST_S

	RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected type '%s'", RFTLE_CSTR( type ) );
	RF_DBGFAIL();

	return reflect::Value{};
}



static bool ProcessProperty( Importer::Callbacks const& callbacks, pugi::xml_node const& prop )
{
	RF_ASSERT( prop.empty() == false );

	// Node checks
	{
		rftl::string_view const name = prop.name();
		pugi::xml_node_type const type = prop.type();

		if( type != pugi::xml_node_type::node_element )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML node type %i", math::integer_cast<int>( math::enum_bitcast( type ) ) );
			return false;
		}

		if( name != "Property" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML non-property node '%s'", RFTLE_CSTR( name ) );
			return false;
		}
	}

	bool keepProcessing = true;

	// New property
	keepProcessing = TryInvoke( callbacks.mInstance_BeginNewPropertyFunc );
	if( keepProcessing == false )
	{
		return false;
	}

	rftl::string_view name = {};
	rftl::string_view type = {};
	rftl::string_view value = {};
	for( pugi::xml_attribute const& attribute : prop.attributes() )
	{
		rftl::string_view const attributeName = attribute.name();
		rftl::string_view const attributeValue = attribute.value();

		if( attributeName == "Name" )
		{
			name = attributeValue;
			continue;
		}

		if( attributeName == "Type" )
		{
			type = attributeValue;
			continue;
		}

		if( attributeName == "Value" )
		{
			value = attributeValue;
			continue;
		}

		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML property attribute name '%s'", RFTLE_CSTR( attributeName ) );
		return false;
	}

	// Name (optional)
	if( name.empty() == false )
	{
		keepProcessing = TryInvoke( callbacks.mProperty_AddNameAttributeFunc, name );
		if( keepProcessing == false )
		{
			return false;
		}
	}

	// Value (optional)
	if( value.empty() == false )
	{
		if( type.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "No type specified for value '%s'", RFTLE_CSTR( value ) );
			return false;
		}

		reflect::Value const asVal = TryConvertValue( type, value );
		if( asVal.GetStoredType() == reflect::Value::Type::Invalid )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Could not convert '%s' to '%s' value", RFTLE_CSTR( name ), RFTLE_CSTR( value ) );
			return false;
		}

		// Value
		keepProcessing = TryInvoke( callbacks.mProperty_AddValueAttributeFunc, asVal );
		if( keepProcessing == false )
		{
			return false;
		}
	}

	return true;
}



static bool ProcessInstance( Importer::Callbacks const& callbacks, pugi::xml_node const& instance )
{
	RF_ASSERT( instance.empty() == false );

	bool keepProcessing = true;

	// New instance
	keepProcessing = TryInvoke( callbacks.mRoot_BeginNewInstanceFunc );
	if( keepProcessing == false )
	{
		return false;
	}

	rftl::string_view instanceID = {};
	rftl::string_view typeID = {};
	for( pugi::xml_attribute const& attribute : instance.attributes() )
	{
		rftl::string_view const attributeName = attribute.name();
		rftl::string_view const attributeValue = attribute.value();

		if( attributeName == "ID" )
		{
			instanceID = attributeValue;
			continue;
		}

		if( attributeName == "TypeID" )
		{
			typeID = attributeValue;
			continue;
		}

		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML instance attribute name '%s'", RFTLE_CSTR( attributeName ) );
		return false;
	}

	// Instance ID (optional)
	if( instanceID.empty() == false )
	{
		exporter::InstanceID val;
		if( rftl::parse_int( val, instanceID ) == false )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Invalid instance ID '%s'", RFTLE_CSTR( instanceID ) );
			return false;
		}

		// Instance ID
		keepProcessing = TryInvoke( callbacks.mInstance_AddInstanceIDAttributeFunc, val );
		if( keepProcessing == false )
		{
			return false;
		}
	}

	// Type ID (optional)
	if( typeID.empty() == false )
	{
		exporter::TypeID val;
		if( rftl::parse_int( val, typeID ) == false )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Invalid type ID '%s'", RFTLE_CSTR( typeID ) );
			return false;
		}

		// Type ID
		keepProcessing = TryInvoke( callbacks.mInstance_AddTypeIDAttributeFunc, val, "TODO_DEBUG_NAME" );
		if( keepProcessing == false )
		{
			return false;
		}
	}

	static constexpr size_t kExcessivePropertyDepth = 10;
	rftl::static_vector<pugi::xml_node, kExcessivePropertyDepth> nodeStack;
	static constexpr size_t kExcessiveIterationLimit = 1000;
	size_t iterationCheckVal = 0;

	// Seed the property tree crawl
	{
		pugi::xml_node seed = instance.first_child();
		if( seed.empty() )
		{
			RFLOG_WARNING( nullptr, RFCAT_SERIALIZATION, "Empty instance, highly dubious" );
		}
		else
		{
			nodeStack.emplace_back( rftl::move( seed ) );
		}
	}

	// Crawl the property tree
	while( true )
	{
		iterationCheckVal++;
		if( iterationCheckVal >= kExcessiveIterationLimit )
		{
			// Excessive iterations, bail
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Property iteration limit %llu exceeded", kExcessiveIterationLimit );
			RF_DBGFAIL();
			return false;
		}

		if( nodeStack.empty() )
		{
			// Finished crawl, leave
			break;
		}

		// Will process at deepest available level
		pugi::xml_node& currentNode = nodeStack.back();

		if( currentNode.empty() )
		{
			// Finished level, ascend and continue

			nodeStack.pop_back();

			if( nodeStack.empty() )
			{
				// Last level (level 0), have now ascended past the initial
				//  seed (level 1), so will not fire the outdent callback,
				//  since that would mean going to a negative property depth
				continue;
			}

			// Outdent
			keepProcessing = TryInvoke( callbacks.mProperty_OutdentFromLastIndentFunc );
			if( keepProcessing == false )
			{
				return false;
			}

			continue;
		}

		// Process node
		keepProcessing = ProcessProperty( callbacks, currentNode );
		if( keepProcessing == false )
		{
			return false;
		}

		// Note any children of this node before moving to next node
		pugi::xml_node child = currentNode.first_child();

		// Move forward to next node in this level
		// NOTE: Want to always do this after processing, even if descending
		currentNode = currentNode.next_sibling();

		// Two iteration paths: continue at current level, or descend
		if( child.empty() == false )
		{
			// Has children, descend and continue

			// Indent
			keepProcessing = TryInvoke( callbacks.mProperty_IndentFromCurrentPropertyFunc );
			if( keepProcessing == false )
			{
				return false;
			}

			nodeStack.emplace_back( rftl::move( child ) );
			continue;
		}
		else
		{
			// No children, continue at current level
			continue;
		}
	}

	return true;
}

}
///////////////////////////////////////////////////////////////////////////////

XmlImporter::XmlImporter() = default;



XmlImporter::~XmlImporter()
{
	if( mFinalized == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Importer destructed without finalizing" );
		RF_DBGFAIL();
	}
}



bool XmlImporter::ReadFromString( rftl::string_view const& string )
{
	if( mFinalized )
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Importer attempting to read after being finalized" );
		RF_DBGFAIL();
		return false;
	}

	pugi::xml_parse_result const result = mDoc.load_buffer(
		string.data(),
		string.size(),
		details::GetCommonParseMode(),
		pugi::encoding_latin1 );

	if( result == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "XML parse error: '%s'", result.description() );
		return false;
	}

	return true;
}



bool XmlImporter::ImportAndFinalize( Callbacks const& callbacks )
{
	if( mFinalized )
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Finalized importer receiving new actions" );
		RF_DBGFAIL();
		return false;
	}

	// No matter what, this call will result in finalization
	mFinalized = true;

	// Locate top-level sections
	{
		RF_ASSERT( mHeader.empty() );
		RF_ASSERT( mTableOfContents.empty() );
		RF_ASSERT( mExternalDependencies.empty() );
		RF_ASSERT( mInternalDependencies.empty() );
		RF_ASSERT( mData.empty() );
		RF_ASSERT( mDebugData.empty() );

		for( pugi::xml_node const& node : mDoc )
		{
			RF_ASSERT( node.empty() == false );

			rftl::string_view const name = node.name();
			pugi::xml_node_type const type = node.type();

			if( type != pugi::xml_node_type::node_element )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML node type %i", math::integer_cast<int>( math::enum_bitcast( type ) ) );
				return false;
			}

			if( name == "Header" )
			{
				mHeader = node;
				continue;
			}

			if( name == "TableOfContents" )
			{
				mTableOfContents = node;
				continue;
			}

			if( name == "ExternalDependencies" )
			{
				mExternalDependencies = node;
				continue;
			}

			if( name == "InternalDependencies" )
			{
				mInternalDependencies = node;
				continue;
			}

			if( name == "Data" )
			{
				mData = node;
				continue;
			}

			if( name == "DebugData" )
			{
				mDebugData = node;
				continue;
			}

			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML node name '%s'", RFTLE_CSTR( name ) );
			return false;
		}

		if( mHeader.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Missing XML header node" );
			return false;
		}

		if( mTableOfContents.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Missing XML TOC node" );
			return false;
		}

		if( mExternalDependencies.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Missing XML external dependencies node" );
			return false;
		}

		if( mInternalDependencies.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Missing XML internal dependencies node" );
			return false;
		}

		if( mData.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Missing XML data node" );
			return false;
		}

		if( mDebugData.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Missing XML debug data node" );
			return false;
		}
	}

	// Check header section
	{
		rftl::string_view magic = {};
		rftl::string_view majorVersion = {};
		rftl::string_view minorVersion = {};
		rftl::string_view subminorVersion = {};

		for( pugi::xml_attribute const& attribute : mHeader.attributes() )
		{
			rftl::string_view const name = attribute.name();
			rftl::string_view const value = attribute.value();

			if( name == "Magic" )
			{
				magic = value;
				continue;
			}

			if( name == "MajorVersion" )
			{
				majorVersion = value;
				continue;
			}

			if( name == "MinorVersion" )
			{
				minorVersion = value;
				continue;
			}

			if( name == "SubminorVersion" )
			{
				subminorVersion = value;
				continue;
			}

			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header attribute name '%s'", RFTLE_CSTR( name ) );
			return false;
		}

		if( magic != "RetroFit Resource" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header magic value '%s'", RFTLE_CSTR( magic ) );
			return false;
		}

		if( majorVersion != "0" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header major value '%s'", RFTLE_CSTR( majorVersion ) );
			return false;
		}

		if( minorVersion != "1" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header minor value '%s'", RFTLE_CSTR( minorVersion ) );
			return false;
		}

		if( subminorVersion != "0" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header subminor value '%s'", RFTLE_CSTR( subminorVersion ) );
			return false;
		}
	}

	// Check TOC
	{
		for( pugi::xml_node const& node : mTableOfContents )
		{
			RF_ASSERT( node.empty() == false );

			rftl::string_view const name = node.name();
			pugi::xml_node_type const type = node.type();

			if( type != pugi::xml_node_type::node_element )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML node type %i", math::integer_cast<int>( math::enum_bitcast( type ) ) );
				return false;
			}

			if( name != "Instance" )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML non-instance node '%s'", RFTLE_CSTR( name ) );
				return false;
			}

			exporter::InstanceID id = exporter::kInvalidInstanceID;
			for( pugi::xml_attribute const& attribute : node.attributes() )
			{
				rftl::string_view const attributeName = attribute.name();
				rftl::string_view const attributeValue = attribute.value();

				if( attributeName != "ID" )
				{
					RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML TOC attribute '%s'", RFTLE_CSTR( attributeName ) );
					return false;
				}

				if( rftl::parse_int( id, attributeValue ) == false )
				{
					RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Invalid instance ID '%s'", RFTLE_CSTR( attributeValue ) );
					return false;
				}
			}

			bool const newEntry = mTocIDs.emplace( id ).second;
			if( newEntry == false )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Duplicate TOC instance ID %llu", id );
				return false;
			}
		}
	}

	// Check external dependencies
	{
		for( pugi::xml_node const& node : mExternalDependencies )
		{
			RF_ASSERT( node.empty() == false );

			RF_TODO_BREAK();
		}
	}

	// Check internal dependencies
	{
		for( pugi::xml_node const& node : mInternalDependencies )
		{
			RF_ASSERT( node.empty() == false );

			RF_TODO_BREAK();
		}
	}

	// Check data
	{
		RF_ASSERT( mRootInstances.empty() );

		for( pugi::xml_node const& node : mData )
		{
			RF_ASSERT( node.empty() == false );

			rftl::string_view const name = node.name();
			pugi::xml_node_type const type = node.type();

			if( type != pugi::xml_node_type::node_element )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML node type %i", math::integer_cast<int>( math::enum_bitcast( type ) ) );
				return false;
			}

			if( name != "Instance" )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML non-instance node '%s'", RFTLE_CSTR( name ) );
				return false;
			}

			mRootInstances.emplace_back( node );
		}

		if( mRootInstances.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "No root instances found" );
			return false;
		}
	}

	// Check debug data
	{
		for( pugi::xml_node const& node : mDebugData )
		{
			RF_ASSERT( node.empty() == false );

			rftl::string_view const name = node.name();
			pugi::xml_node_type const type = node.type();

			if( type != pugi::xml_node_type::node_element )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML node type %i", math::integer_cast<int>( math::enum_bitcast( type ) ) );
				return false;
			}

			if( name != "TypeID" )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML non-type node '%s'", RFTLE_CSTR( name ) );
				return false;
			}

			rftl::string_view value = {};
			rftl::string_view debugName = {};
			for( pugi::xml_attribute const& attribute : node.attributes() )
			{
				rftl::string_view const attributeName = attribute.name();
				rftl::string_view const attributeValue = attribute.value();

				if( attributeName == "Value" )
				{
					value = attributeValue;
					continue;
				}

				if( attributeName == "DebugName" )
				{
					debugName = attributeValue;
					continue;
				}

				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML debug attribute name '%s'", RFTLE_CSTR( attributeName ) );
				return false;
			}

			exporter::TypeID id;
			if( rftl::parse_int( id, value ) == false )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Invalid type ID '%s'", RFTLE_CSTR( value ) );
				return false;
			}

			if( debugName.empty() )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Empty debug name for type ID %llu", id );
				return false;
			}

			bool const newEntry = mDebugNames.emplace( id, debugName ).second;
			if( newEntry == false )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Duplicate debug name for type ID %llu", id );
				return false;
			}
		}
	}

	// TODO: Emit indirections
	RF_TODO_ANNOTATION( "Emit indirections" );
	( (void)callbacks.mRoot_RegisterLocalIndirectionFunc );
	( (void)callbacks.mRoot_RegisterExternalIndirectionFunc );

	// Process the instances
	for( pugi::xml_node const& instance : mRootInstances )
	{
		bool const keepProcessing = details::ProcessInstance( callbacks, instance );
		if( keepProcessing == false )
		{
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
