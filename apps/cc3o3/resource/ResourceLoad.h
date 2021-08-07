#pragma once
#include "cc3o3/resource/ResourceFwd.h"

#include "PlatformFilesystem/VFSFwd.h"


namespace RF::cc::resource {
///////////////////////////////////////////////////////////////////////////////

void InitializeLoader();

template<typename T>
UniquePtr<T> LoadFromFile( file::VFSPath const& path );

///////////////////////////////////////////////////////////////////////////////
}
