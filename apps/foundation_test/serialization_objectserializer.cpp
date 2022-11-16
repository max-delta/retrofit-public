#include "stdafx.h"

#include "Serialization/ObjectSerializer.h"
#include "Serialization/DiagnosticExporter.h"
#include "Serialization/XmlExporter.h"
#include "RFType/Example.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, ObjectSerializerSanityCheck )
{
	rftype_example::ExampleWithoutStaticClassInfo obj = {};

	{
		DiagnosticExporter diag{ true };

		bool const serializeSuccess = ObjectSerializer::SerializeSingleObject( diag, rftype::GetClassInfo<decltype( obj )>(), &obj );
		ASSERT_TRUE( serializeSuccess );

		bool const finalizeSuccess = diag.Root_FinalizeExport();
		ASSERT_TRUE( finalizeSuccess );
	}

	{
		XmlExporter xml{};

		bool const serializeSuccess = ObjectSerializer::SerializeSingleObject( xml, rftype::GetClassInfo<decltype( obj )>(), &obj );
		ASSERT_TRUE( serializeSuccess );

		bool const finalizeSuccess = xml.Root_FinalizeExport();
		ASSERT_TRUE( finalizeSuccess );

		rftl::string string;
		ASSERT_TRUE( xml.WriteToString( string ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
