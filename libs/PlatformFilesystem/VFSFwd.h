#pragma once
#include "core/ptr/unique_ptr.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

class VFS;
class VFSPath;
class VFSMount;
class FileHandle;

typedef UniquePtr<FileHandle> FileHandlePtr;

constexpr char kPathDelimiter = '/';
constexpr char kPathAscensionElement[] = "..";
constexpr char kPathCurrentElement[] = ".";

///////////////////////////////////////////////////////////////////////////////
}}
