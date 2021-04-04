#pragma once
#include "project.h"

#include "cc3o3/save/SaveFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/unordered_set"


namespace RF::cc::save {
///////////////////////////////////////////////////////////////////////////////

class SaveManager
{
	RF_NO_COPY( SaveManager );

	//
	// Types and constants
public:
	using SaveName = rftl::string;
	using SaveNames = rftl::unordered_set<SaveName>;

	//
	// Public methods
public:
	SaveManager() = default;
	~SaveManager() = default;

	SaveNames FindSaveNames() const;

	UniquePtr<SaveBlob> LoadSave( SaveName const& name ) const;
};

///////////////////////////////////////////////////////////////////////////////
}
