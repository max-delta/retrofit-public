#pragma once
#include "cc3o3/resource/ResourceFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/vector"


namespace RF::cc::resource {
///////////////////////////////////////////////////////////////////////////////

void InitializeLoader();

template<typename T>
UniquePtr<T> LoadFromFile( file::VFSPath const& path );

template<typename T>
rftl::vector<UniquePtr<T>> LoadFromDirectory( file::VFSPath const& path, bool recursive );

///////////////////////////////////////////////////////////////////////////////
}
