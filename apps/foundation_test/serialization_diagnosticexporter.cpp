#include "stdafx.h"

#include "Serialization/DiagnosticExporter.h"


namespace RF { namespace serialization {
///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, DiagnosticExporterSanityCheck )
{
	DiagnosticExporter e = DiagnosticExporter( true );
	ASSERT_TRUE( e.Root_BeginNewInstance() );
	{
		ASSERT_TRUE( e.Instance_AddInstanceIDAttribute( 3 ) );
		ASSERT_TRUE( e.Instance_AddTypeIDAttribute( 2, "2" ) );

		ASSERT_TRUE( e.Instance_BeginNewProperty() );
		ASSERT_TRUE( e.Property_AddNameAttribute( "4" ) );
		ASSERT_TRUE( e.Property_AddValueAttribute( reflect::Value( 4 ) ) );

		ASSERT_TRUE( e.Instance_BeginNewProperty() );
		ASSERT_TRUE( e.Property_AddNameAttribute( "5" ) );
		ASSERT_TRUE( e.Property_IndentFromCurrentProperty() );
		{
			ASSERT_TRUE( e.Instance_BeginNewProperty() );
			ASSERT_TRUE( e.Property_AddNameAttribute( "6" ) );
			ASSERT_TRUE( e.Property_AddIndirectionAttribute( 1 ) );
		}
		ASSERT_TRUE( e.Property_OutdentFromLastIndent() );

		ASSERT_TRUE( e.Instance_BeginNewProperty() );
		ASSERT_TRUE( e.Property_AddNameAttribute( "7" ) );
		ASSERT_TRUE( e.Property_AddValueAttribute( reflect::Value( 7 ) ) );
	}
	ASSERT_TRUE( e.Root_RegisterExternalIndirection( 1, "1" ) );
	ASSERT_TRUE( e.Root_RegisterLocalIndirection( 0, 0 ) );

	ASSERT_TRUE( e.Root_FinalizeExport() );
}

///////////////////////////////////////////////////////////////////////////////
}}
