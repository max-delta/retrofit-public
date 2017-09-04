#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSPath.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API VFSMount
{
private:
	friend class VFS;
public:
	enum class Type : uint8_t
	{
		Invalid = 0,
		Absolute,
		ConfigRelative,
		UserRelative
	};
	enum class Permissions : uint8_t
	{
		Invalid = 0,
		ReadOnly,
		ReadWrite,
		ReadExecute,
	};
private:
	Type m_Type;
	Permissions m_Permissions;
	VFSPath m_VirtualPath;
	VFSPath m_RealMount;
};

///////////////////////////////////////////////////////////////////////////////
}}
