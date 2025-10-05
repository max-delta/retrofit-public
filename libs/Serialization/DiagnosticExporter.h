#pragma once
#include "project.h"

#include "Serialization/Exporter.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API DiagnosticExporter final : public Exporter
{
	RF_NO_COPY( DiagnosticExporter );

public:
	DiagnosticExporter( bool silent );
	~DiagnosticExporter() override;

	virtual bool Root_FinalizeExport() override;

	virtual bool Root_BeginNewInstance() override;
	virtual bool Root_RegisterLocalIndirection( IndirectionID const& indirectionID, InstanceID const& instanceID ) override;
	virtual bool Root_RegisterExternalIndirection( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID ) override;

	virtual bool Instance_AddInstanceIDAttribute( InstanceID const& instanceID ) override;
	virtual bool Instance_AddTypeIDAttribute( TypeID const& typeID, rftl::cstring_view debugName ) override;
	virtual bool Instance_BeginNewProperty() override;

	virtual bool Property_AddNameAttribute( rftl::cstring_view name ) override;
	virtual bool Property_AddValueAttribute( reflect::Value const& value ) override;
	virtual bool Property_AddDebugTypeIDAttribute( TypeID const& debugTypeID, rftl::cstring_view debugName ) override;
	virtual bool Property_AddIndirectionAttribute( IndirectionID const& indirectionID ) override;
	virtual bool Property_IndentFromCurrentProperty() override;
	virtual bool Property_OutdentFromLastIndent() override;


private:
	bool mFinalized;
	bool mSilent;
	bool mHasOpenInstance;
	bool mHasOpenProperty;
	size_t mPropertyDepth;
};

///////////////////////////////////////////////////////////////////////////////
}
