#include "stdafx.h"
#include "DiagnosticImportTestShim.h"

#include "Serialization/Importer.h"
#include "Serialization/DiagnosticExporter.h"

#include "core_math/math_casts.h"
#include "core_reflect/Value.h"

RF_TODO_ANNOTATION( "Remove c_str usages, revise API" );
#include "rftl/extension/c_str.h"

#include "rftl/string"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

bool DiagnosticProcessImport( Importer& importer )
{
	DiagnosticExporter diagExporter{ false };

	using InstanceID = Importer::InstanceID;
	using TypeID = Importer::TypeID;
	using IndirectionID = Importer::IndirectionID;
	using ExternalReferenceID = Importer::ExternalReferenceID;

	Importer::Callbacks callbacks = {};

	callbacks.mRoot_BeginNewInstanceFunc =
		[&diagExporter]() -> bool
	{
		return diagExporter.Root_BeginNewInstance();
	};

	callbacks.mRoot_RegisterLocalIndirectionFunc =
		[&diagExporter]( IndirectionID const& indirectionID, InstanceID const& instanceID ) -> bool
	{
		return diagExporter.Root_RegisterLocalIndirection( indirectionID, instanceID );
	};

	callbacks.mRoot_RegisterExternalIndirectionFunc =
		[&diagExporter]( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID ) -> bool
	{
		return diagExporter.Root_RegisterExternalIndirection( indirectionID, referenceID );
	};

	callbacks.mInstance_AddInstanceIDAttributeFunc =
		[&diagExporter]( InstanceID const& instanceID ) -> bool
	{
		return diagExporter.Instance_AddInstanceIDAttribute( instanceID );
	};

	callbacks.mInstance_AddTypeIDAttributeFunc =
		[&diagExporter]( TypeID const& typeID, char const* debugName ) -> bool
	{
		return diagExporter.Instance_AddTypeIDAttribute( typeID, debugName );
	};

	callbacks.mInstance_BeginNewPropertyFunc =
		[&diagExporter]() -> bool
	{
		return diagExporter.Instance_BeginNewProperty();
	};

	callbacks.mProperty_AddNameAttributeFunc =
		[&diagExporter]( rftl::string_view const& name ) -> bool
	{
		return diagExporter.Property_AddNameAttribute( RFTLE_CSTR( name ) );
	};

	callbacks.mProperty_AddValueAttributeFunc =
		[&diagExporter]( reflect::Value const& value ) -> bool
	{
		return diagExporter.Property_AddValueAttribute( value );
	};

	callbacks.mProperty_AddIndirectionAttributeFunc =
		[&diagExporter]( IndirectionID const& indirectionID ) -> bool
	{
		return diagExporter.Property_AddIndirectionAttribute( indirectionID );
	};

	callbacks.mProperty_IndentFromCurrentPropertyFunc =
		[&diagExporter]() -> bool
	{
		return diagExporter.Property_IndentFromCurrentProperty();
	};

	callbacks.mProperty_OutdentFromLastIndentFunc =
		[&diagExporter]() -> bool
	{
		return diagExporter.Property_OutdentFromLastIndent();
	};

	bool const success = importer.ImportAndFinalize( callbacks );

	// Regardless of success or failure, try to finalize the exporter
	bool const finalized = diagExporter.Root_FinalizeExport();

	return success && finalized;
}

///////////////////////////////////////////////////////////////////////////////
}
