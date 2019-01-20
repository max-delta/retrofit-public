#include "stdafx.h"
#include "DiagnosticExporter.h"

#include "Logging/Logging.h"

#include "rftl/string"


#define RF_DIAG_EXP_TR( ... ) \
	if( mSilent == false ) \
	{ \
		RFLOG_MILESTONE( nullptr, RFCAT_SERIALIZATION, __VA_ARGS__ ); \
	}

namespace RF { namespace serialization {
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

	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %s value := VAL", indents.c_str() );

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

	mPropertyDepth--;
	rftl::string indents( mPropertyDepth * 2, ' ' );
	RF_DIAG_EXP_TR( " %s }", indents.c_str() );

	mHasOpenProperty = false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}

#undef RF_DIAG_EXP_TR
