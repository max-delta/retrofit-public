#include "stdafx.h"
#include "XmlImporter.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"

#include "rftl/extension/c_str.h"
#include "rftl/extension/string_parse.h"
#include "rftl/optional"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// HACK: Can't be constexpr, since source values are non-constexpr
static unsigned int getCommonParseMode()
{
	// Currently don't have a reason to enable any wierd XML features
	return pugi::parse_minimal;
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
		details::getCommonParseMode(),
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

			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML node name '%s'", rftl::c_str( name ) );
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

			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header attribute name '%s'", rftl::c_str( name ) );
			return false;
		}

		if( magic != "RetroFit Resource" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header magic value '%s'", rftl::c_str( magic ) );
			return false;
		}

		if( majorVersion != "0" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header major value '%s'", rftl::c_str( majorVersion ) );
			return false;
		}

		if( minorVersion != "1" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header minor value '%s'", rftl::c_str( minorVersion ) );
			return false;
		}

		if( subminorVersion != "0" )
		{
			RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML header subminor value '%s'", rftl::c_str( subminorVersion ) );
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
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML non-instance node '%s'", rftl::c_str( name ) );
				return false;
			}

			exporter::InstanceID id = exporter::kInvalidInstanceID;
			for( pugi::xml_attribute const& attribute : node.attributes() )
			{
				rftl::string_view const attributeName = attribute.name();
				rftl::string_view const attributeValue = attribute.value();

				if( attributeName != "ID" )
				{
					RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML TOC attribute '%s'", rftl::c_str( attributeName ) );
					return false;
				}

				if( rftl::parse_int( id, attributeValue ) == false )
				{
					RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Invalid instance ID '%s'", rftl::c_str( attributeValue ) );
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
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML non-instance node '%s'", rftl::c_str( name ) );
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
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML non-type node '%s'", rftl::c_str( name ) );
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

				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unexpected XML debug attribute name '%s'", rftl::c_str( attributeName ) );
				return false;
			}

			exporter::TypeID id;
			if( rftl::parse_int( id, value ) == false )
			{
				RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Invalid type ID '%s'", rftl::c_str( value ) );
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

	// TODO: Begin processing the instances
	RF_TODO_BREAK();
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}
