#pragma once
#include "core/ptr/unique_ptr.h"

#include "rftl/limits"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

class VFS;
class VFSPath;
class VFSMount;
class FileHandle;

using FileHandlePtr = UniquePtr<FileHandle>;

// Lower numbers are more important
using MountPriority = uint8_t;
static constexpr MountPriority kMountPriorityHighest = 0;
static constexpr MountPriority kMountPriorityLowest = rftl::numeric_limits<MountPriority>::max();

static constexpr char kPathDelimiter = '/';
static constexpr char kPathAscensionElement[] = "..";
static constexpr char kPathCurrentElement[] = ".";

///////////////////////////////////////////////////////////////////////////////
}
