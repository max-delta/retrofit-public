#pragma once
#include "project.h"

#include "Serialization/Importer.h"

#include "core/macros.h"
#include "core/ptr/unique_ptr.h"

#include "rftl/string_view"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API AutoImporter final : public Importer
{
	RF_NO_COPY( AutoImporter );

public:
	AutoImporter();
	~AutoImporter() override;

	bool ReadFromString( rftl::string_view const& string );

	virtual bool ImportAndFinalize( Callbacks const& callbacks ) override;


private:
	UniquePtr<Importer> mUnderlying;
};

///////////////////////////////////////////////////////////////////////////////
}
