#pragma once
#include "project.h"

#include "Serialization/Importer.h"

#include "core/macros.h"
#include "rftl/string_view"
#include "rftl/optional"
#include "rftl/unordered_map"
#include "rftl/vector"

#include <pugixml/pugixml.h>


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API XmlImporter final : public Importer
{
	RF_NO_COPY( XmlImporter );

private:
	using TocEntries = rftl::unordered_map<exporter::InstanceID, rftl::optional<exporter::TypeID>>;
	using LocalIndirections = rftl::unordered_map<exporter::IndirectionID, exporter::InstanceID>;
	using RootInstances = rftl::vector<pugi::xml_node>;
	using DebugNames = rftl::unordered_map<exporter::TypeID, rftl::string_view>;


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

	TocEntries mTocEntries;
	LocalIndirections mLocalIndirections;
	RootInstances mRootInstances;
	DebugNames mDebugNames;

	pugi::xml_node mCurrentInstance;
	rftl::vector<pugi::xml_node> mPropertyStack;

	bool mFinalized = false;
};

///////////////////////////////////////////////////////////////////////////////
}
