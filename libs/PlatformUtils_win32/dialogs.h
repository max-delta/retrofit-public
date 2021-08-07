#pragma once
#include "project.h"

#include "core_platform/shim/win_shim.h"

#include "rftl/string"
#include "rftl/vector"


namespace RF::platform::dialogs {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Filters are of the form { "Myfile", "*.mf" }
// NOTE: The filter { "All", "*.*" } is always available to users
using FileFilter = rftl::pair<rftl::string, rftl::string>;
using FileFilters = rftl::vector<FileFilter>;

PLATFORMUTILS_API rftl::string OpenFileDialog( FileFilters filters = {} );
PLATFORMUTILS_API rftl::string SaveFileDialog( FileFilters filters = {} );

///////////////////////////////////////////////////////////////////////////////
}
