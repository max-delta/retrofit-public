#pragma once
#include "project.h"

#include "Serialization/Exporter.h"

#include "core/macros.h"
#include "rftl/string"
#include "rftl/vector"

#include <pugixml/pugixml.h>


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API XmlExporter final : public Exporter
{
	RF_NO_COPY( XmlExporter );

public:
	XmlExporter();
	~XmlExporter() override;

	bool WriteToFile( FILE* file ) const;
	bool WriteToString( rftl::string& string ) const;

	virtual bool Root_FinalizeExport() override;

	virtual bool Root_BeginNewInstance() override;
	virtual bool Root_RegisterLocalIndirection( IndirectionID const& indirectionID, InstanceID const& instanceID ) override;
	virtual bool Root_RegisterExternalIndirection( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID ) override;

	virtual bool Instance_AddInstanceIDAttribute( InstanceID const& instanceID ) override;
	virtual bool Instance_AddTypeIDAttribute( TypeID const& typeID, char const* debugName ) override;
	virtual bool Instance_BeginNewProperty() override;

	virtual bool Property_AddNameAttribute( char const* name ) override;
	virtual bool Property_AddValueAttribute( reflect::Value const& value ) override;
	virtual bool Property_AddIndirectionAttribute( IndirectionID const& indirectionID ) override;
	virtual bool Property_IndentFromCurrentProperty() override;
	virtual bool Property_OutdentFromLastIndent() override;


private:
	pugi::xml_document mDoc;

	pugi::xml_node mHeader;
	pugi::xml_node mTableOfContents;
	pugi::xml_node mExternalDependencies;
	pugi::xml_node mInternalDependencies;
	pugi::xml_node mData;
	pugi::xml_node mDebugData;

	pugi::xml_node mCurrentInstance;
	rftl::vector<pugi::xml_node> mPropertyStack;
	bool mNewIndent = false;
};

///////////////////////////////////////////////////////////////////////////////
}
