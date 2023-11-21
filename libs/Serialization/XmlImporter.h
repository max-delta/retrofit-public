#pragma once
#include "project.h"

#include "Serialization/Importer.h"

#include "core/macros.h"
#include "rftl/string_view"
#include "rftl/unordered_set"
#include "rftl/unordered_map"
#include "rftl/vector"

#include <pugixml/pugixml.h>


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API XmlImporter final : public Importer
{
	RF_NO_COPY( XmlImporter );

public:
	XmlImporter();
	~XmlImporter() override;

	bool ReadFromString( rftl::string_view const& string );

	virtual bool ImportAndFinalize( Callbacks const& callbacks ) override;


private:
	pugi::xml_document mDoc;

	pugi::xml_node mHeader;
	pugi::xml_node mTableOfContents;
	pugi::xml_node mExternalDependencies;
	pugi::xml_node mInternalDependencies;
	pugi::xml_node mData;
	pugi::xml_node mDebugData;

	rftl::unordered_set<exporter::InstanceID> mTocIDs;
	rftl::unordered_map<exporter::IndirectionID, exporter::InstanceID> mInternalDepIDs;
	rftl::vector<pugi::xml_node> mRootInstances;
	rftl::unordered_map<exporter::TypeID, rftl::string_view> mDebugNames;

	pugi::xml_node mCurrentInstance;
	rftl::vector<pugi::xml_node> mPropertyStack;

	bool mFinalized = false;
};

///////////////////////////////////////////////////////////////////////////////
}
