#pragma once
#include "project.h"

#include "Serialization/SerializationFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/macros.h"

#include "rftl/functional"
#include "rftl/string_view"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API Importer
{
	RF_NO_COPY( Importer );

public:
	using InstanceID = exporter::InstanceID;
	using TypeID = exporter::TypeID;
	using IndirectionID = exporter::IndirectionID;
	using ExternalReferenceID = exporter::ExternalReferenceID;

	// Callbacks meant to match the inverse of calling into an exporter
	// SEE: RF::serialization::Exporter
	struct Callbacks
	{
		using Root_BeginNewInstanceFuncSig = bool();
		using Root_RegisterLocalIndirectionFuncSig = bool( IndirectionID const& indirectionID, InstanceID const& instanceID );
		using Root_RegisterExternalIndirectionFuncSig = bool( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID );
		using Instance_AddInstanceIDAttributeFuncSig = bool( InstanceID const& instanceID );
		using Instance_AddTypeIDAttributeFuncSig = bool( TypeID const& typeID, char const* debugName );
		using Instance_BeginNewPropertyFuncSig = bool();
		using Property_AddNameAttributeFuncSig = bool( rftl::string_view const& name );
		using Property_AddValueAttributeFuncSig = bool( reflect::Value const& value );
		using Property_AddIndirectionAttributeFuncSig = bool( IndirectionID const& indirectionID );
		using Property_IndentFromCurrentPropertyFuncSig = bool();
		using Property_OutdentFromLastIndentFuncSig = bool();

		using Root_BeginNewInstanceFunc = rftl::function<Root_BeginNewInstanceFuncSig>;
		using Root_RegisterLocalIndirectionFunc = rftl::function<Root_RegisterLocalIndirectionFuncSig>;
		using Root_RegisterExternalIndirectionFunc = rftl::function<Root_RegisterExternalIndirectionFuncSig>;
		using Instance_AddInstanceIDAttributeFunc = rftl::function<Instance_AddInstanceIDAttributeFuncSig>;
		using Instance_AddTypeIDAttributeFunc = rftl::function<Instance_AddTypeIDAttributeFuncSig>;
		using Instance_BeginNewPropertyFunc = rftl::function<Instance_BeginNewPropertyFuncSig>;
		using Property_AddNameAttributeFunc = rftl::function<Property_AddNameAttributeFuncSig>;
		using Property_AddValueAttributeFunc = rftl::function<Property_AddValueAttributeFuncSig>;
		using Property_AddIndirectionAttributeFunc = rftl::function<Property_AddIndirectionAttributeFuncSig>;
		using Property_IndentFromCurrentPropertyFunc = rftl::function<Property_IndentFromCurrentPropertyFuncSig>;
		using Property_OutdentFromLastIndentFunc = rftl::function<Property_OutdentFromLastIndentFuncSig>;

		Root_BeginNewInstanceFunc mRoot_BeginNewInstanceFunc = {};
		Root_RegisterLocalIndirectionFunc mRoot_RegisterLocalIndirectionFunc = {};
		Root_RegisterExternalIndirectionFunc mRoot_RegisterExternalIndirectionFunc = {};
		Instance_AddInstanceIDAttributeFunc mInstance_AddInstanceIDAttributeFunc = {};
		Instance_AddTypeIDAttributeFunc mInstance_AddTypeIDAttributeFunc = {};
		Instance_BeginNewPropertyFunc mInstance_BeginNewPropertyFunc = {};
		Property_AddNameAttributeFunc mProperty_AddNameAttributeFunc = {};
		Property_AddValueAttributeFunc mProperty_AddValueAttributeFunc = {};
		Property_AddIndirectionAttributeFunc mProperty_AddIndirectionAttributeFunc = {};
		Property_IndentFromCurrentPropertyFunc mProperty_IndentFromCurrentPropertyFunc = {};
		Property_OutdentFromLastIndentFunc mProperty_OutdentFromLastIndentFunc = {};
	};


public:
	Importer() = default;
	virtual ~Importer() = default;

	// Attempts an import, which will fire callbacks as it processes, then
	//  finalize the importer preventing any further actions from being
	//  performed on it
	// NOTE: Returns false on failure
	// NOTE: Returning false from any callback will terminate the import and
	//  progate the false return value to the original caller
	// NOTE: Finalization occurs regardless of the return value
	virtual bool ImportAndFinalize( Callbacks const& callbacks ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
