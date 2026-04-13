#pragma once

#include "core_vfs/VFSFwd.h"

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
inline constexpr MountPriority kMountPriorityHighest = 0;
inline constexpr MountPriority kMountPriorityLowest = rftl::numeric_limits<MountPriority>::max();

inline constexpr char kPathDelimiter = '/';
inline constexpr char kPathAscensionElement[] = "..";
inline constexpr char kPathCurrentElement[] = ".";

enum class OpenFlags : uint8_t
{
	Invalid = 0,
	Read, //rb
	Write, //wb+
	Modify, //rb+
	Append, //ab+
	Execute, //rb
};

///////////////////////////////////////////////////////////////////////////////
}
