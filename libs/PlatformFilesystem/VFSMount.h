#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSPath.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API VFSMount
{
	//
	// Friends
private:
	friend class VFS;


	//
	// Constants
public:
	static constexpr uint8_t kReadBit = 1 << 2;
	static constexpr uint8_t kWriteBit = 1 << 1;
	static constexpr uint8_t kExecuteBit = 1 << 0;


	//
	// Enums
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
		ReadOnly = kReadBit,
		ReadWrite = kReadBit | kWriteBit,
		ReadExecute = kReadBit | kExecuteBit
	};


	//
	// Private data
private:
	MountPriority mPriority = 0;
	Type mType = Type::Invalid;
	Permissions mPermissions = Permissions::Invalid;
	VFSPath mVirtualPath = {};
	VFSPath mRealMount = {};
};

///////////////////////////////////////////////////////////////////////////////
}
