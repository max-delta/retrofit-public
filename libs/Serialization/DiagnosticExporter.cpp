#include "stdafx.h"
#include "DiagnosticExporter.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"
#include "core_reflect/Value.h"

#include "rftl/string"


#define RF_DIAG_EXP_TR( ... ) \
	if( mSilent == false ) \
	{ \
		RFLOG_MILESTONE( nullptr, RFCAT_SERIALIZATION, __VA_ARGS__ ); \
	}

namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

DiagnosticExporter::DiagnosticExporter( bool silent )
	: mFinalized( false )
	, mSilent( silent )
	, mHasOpenInstance( false )
	, mHasOpenProperty( false )
	, mPropertyDepth( 0 )
{
	RF_DIAG_EXP_TR( "<OPEN>" );
}



DiagnosticExporter::~DiagnosticExporter()
{
	RFLOG_TEST_AND_NOTIFY( mFinalized, nullptr, RFCAT_SERIALIZATION, "Exporter destructed without finalizing" );
}



bool DiagnosticExporter::Root_FinalizeExport()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter attempting to re-finalize" );
		return false;
	}

	RF_DIAG_EXP_TR( "<CLOSE>" );
	mFinalized = true;
	mHasOpenInstance = false;
	mHasOpenProperty = false;
	return true;
}



bool DiagnosticExporter::Root_BeginNewInstance()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	RF_DIAG_EXP_TR( "{" );
	mHasOpenInstance = true;
	mHasOpenProperty = false;
	return true;
}



bool DiagnosticExporter::Root_RegisterLocalIndirection( IndirectionID const& indirectionID, InstanceID const& instanceID )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	RF_DIAG_EXP_TR( "(%llu := %lli)", indirectionID, instanceID );

	return true;
}



bool DiagnosticExporter::Root_RegisterExternalIndirection( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	RF_DIAG_EXP_TR( "(%llu := \"%s\")", indirectionID, referenceID );

	return true;
}



bool DiagnosticExporter::Instance_AddInstanceIDAttribute( InstanceID const& instanceID )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenInstance == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Instance operation without open instance" );
		return false;
	}

	RF_DIAG_EXP_TR( " InstId := %llu", instanceID );

	return true;
}



bool DiagnosticExporter::Instance_AddTypeIDAttribute( TypeID const& typeID, char const* debugName )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenInstance == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Instance operation without open instance" );
		return false;
	}

	if( mHasOpenProperty )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Instance type ID cannot be added after properties have already started" );
		return false;
	}

	if( debugName != nullptr )
	{
		RF_DIAG_EXP_TR( " TypeId := %llu (\"%s\")", typeID, debugName );
	}
	else
	{
		RF_DIAG_EXP_TR( " TypeId := %llu", typeID );
	}

	return true;
}



bool DiagnosticExporter::Instance_BeginNewProperty()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenInstance == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Instance operation without open instance" );
		return false;
	}

	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %sL", indents.c_str() );

	mHasOpenProperty = true;
	return true;
}



bool DiagnosticExporter::Property_AddNameAttribute( char const* name )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenProperty == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Property operation without open property" );
		return false;
	}

	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %s name := %s", indents.c_str(), name );

	return true;
}



bool DiagnosticExporter::Property_AddValueAttribute( reflect::Value const& value )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenProperty == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Property operation without open property" );
		return false;
	}

	char const* typeName = nullptr;
	rftl::string valueStr;
	{
		reflect::Value::Type const type = value.GetStoredType();

		if( type != reflect::Value::Type::Invalid )
		{
			typeName = reflect::Value::GetTypeName( type );
		}
		else
		{
			typeName = "INVALID";
		}

		switch( type )
		{
			case reflect::Value::Type::Bool:
				valueStr = *value.GetAs<bool>() ? "true" : "false";
				break;
			case reflect::Value::Type::VoidPtr:
				valueStr = "<PTR>";
				break;
			case reflect::Value::Type::VoidConstPtr:
				valueStr = "<PTR>";
				break;
			case reflect::Value::Type::VirtualClassPtr:
				valueStr = "<PTR>";
				break;
			case reflect::Value::Type::VirtualClassConstPtr:
				valueStr = "<PTR>";
				break;
			case reflect::Value::Type::Char:
				valueStr = rftl::to_string( math::integer_cast<int>( *value.GetAs<char>() ) );
				break;
			case reflect::Value::Type::WChar:
				valueStr = rftl::to_string( math::integer_cast<int>( *value.GetAs<wchar_t>() ) );
				break;
			case reflect::Value::Type::Char16:
				valueStr = rftl::to_string( math::integer_cast<int>( *value.GetAs<char16_t>() ) );
				break;
			case reflect::Value::Type::Char32:
				valueStr = rftl::to_string( math::integer_cast<int>( *value.GetAs<char32_t>() ) );
				break;
			case reflect::Value::Type::Float:
				valueStr = rftl::to_string( *value.GetAs<float>() );
				break;
			case reflect::Value::Type::Double:
				valueStr = rftl::to_string( *value.GetAs<double>() );
				break;
			case reflect::Value::Type::LongDouble:
				valueStr = rftl::to_string( *value.GetAs<long double>() );
				break;
			case reflect::Value::Type::UInt8:
				valueStr = rftl::to_string( *value.GetAs<uint8_t>() );
				break;
			case reflect::Value::Type::UInt16:
				valueStr = rftl::to_string( *value.GetAs<uint16_t>() );
				break;
			case reflect::Value::Type::UInt32:
				valueStr = rftl::to_string( *value.GetAs<uint32_t>() );
				break;
			case reflect::Value::Type::UInt64:
				valueStr = rftl::to_string( *value.GetAs<uint64_t>() );
				break;
			case reflect::Value::Type::Int8:
				valueStr = rftl::to_string( *value.GetAs<int8_t>() );
				break;
			case reflect::Value::Type::Int16:
				valueStr = rftl::to_string( *value.GetAs<int16_t>() );
				break;
			case reflect::Value::Type::Int32:
				valueStr = rftl::to_string( *value.GetAs<int32_t>() );
				break;
			case reflect::Value::Type::Int64:
				valueStr = rftl::to_string( *value.GetAs<int64_t>() );
				break;
			case reflect::Value::Type::Invalid:
			default:
				valueStr = "<UNSUPPORTED>";
				break;
		}
	}

	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %s value := [%s] %s", indents.c_str(), typeName, valueStr.c_str() );

	return true;
}



bool DiagnosticExporter::Property_AddDebugTypeIDAttribute( TypeID const& debugTypeID, char const* debugName )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenProperty == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Property operation without open property" );
		return false;
	}

	rftl::string indents( mPropertyDepth * 2, ' ' );
	if( debugName != nullptr )
	{
		RF_DIAG_EXP_TR( " %s Debug TypeId := %llu (\"%s\")", indents.c_str(), debugTypeID, debugName );
	}
	else
	{
		RF_DIAG_EXP_TR( " %s Debug TypeId := %llu", indents.c_str(), debugTypeID );
	}

	return true;
}



bool DiagnosticExporter::Property_AddIndirectionAttribute( IndirectionID const& indirectionID )
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenProperty == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Property operation without open property" );
		return false;
	}

	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %s indirection := %llu", indents.c_str(), indirectionID );

	return true;
}



bool DiagnosticExporter::Property_IndentFromCurrentProperty()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mHasOpenProperty == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Property operation without open property" );
		return false;
	}

	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %s {", indents.c_str() );
	mPropertyDepth++;

	mHasOpenProperty = false;
	return true;
}



bool DiagnosticExporter::Property_OutdentFromLastIndent()
{
	if( mFinalized )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Finalized exporter receiving new actions" );
		return false;
	}

	if( mPropertyDepth == 0 )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SERIALIZATION, "Property depth rollover" );
		return false;
	}
	mPropertyDepth--;

	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %s }", indents.c_str() );

	mHasOpenProperty = false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

#undef RF_DIAG_EXP_TR
