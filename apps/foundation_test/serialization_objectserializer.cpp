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
		bool const success = ObjectSerializer::SerializeSingleObject( diag, rftype::GetClassInfo<decltype( obj )>(), &obj );
		ASSERT_TRUE( success );
	}

	{
		XmlExporter xml{};
		bool const success = ObjectSerializer::SerializeSingleObject( xml, rftype::GetClassInfo<decltype( obj )>(), &obj );
		ASSERT_TRUE( success );
		rftl::string string;
		ASSERT_TRUE( xml.WriteToString( string ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
