#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core_logging/LoggingFwd.h"

#include "rftl/format"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

PLATFORMFILESYSTEM_API void FileLogger( logging::LoggingRouter const& router, logging::LogEvent<char8_t> const& event, rftl::format_args const& args );

PLATFORMFILESYSTEM_API void SetLogFileDirectory( VFS& vfs, VFSPath const& directory );
PLATFORMFILESYSTEM_API void UnsetLogFileDirectory();

///////////////////////////////////////////////////////////////////////////////
}
