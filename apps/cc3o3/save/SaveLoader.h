#pragma once
#include "project.h"

#include "cc3o3/save/SaveFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string_view"


namespace RF::cc::save {
///////////////////////////////////////////////////////////////////////////////

class SaveLoader
{
	RF_NO_COPY( SaveLoader );

	//
	// Public methods
public:
	SaveLoader() = default;
	~SaveLoader() = default;

	UniquePtr<SaveBlob> LoadFromBuffer( rftl::string_view buffer ) const;
};

///////////////////////////////////////////////////////////////////////////////
}
