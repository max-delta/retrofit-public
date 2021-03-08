#pragma once
#include "core/ptr/unique_ptr.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

class VFS;
class VFSPath;
class VFSMount;
class FileHandle;

using FileHandlePtr = UniquePtr<FileHandle>;

constexpr char kPathDelimiter = '/';
constexpr char kPathAscensionElement[] = "..";
constexpr char kPathCurrentElement[] = ".";

///////////////////////////////////////////////////////////////////////////////
}
