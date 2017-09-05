#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSPath.h"


namespace RF { namespace file {
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
	static constexpr uint8_t k_ReadBit = 1 << 2;
	static constexpr uint8_t k_WriteBit = 1 << 1;
	static constexpr uint8_t k_ExecuteBit = 1 << 0;


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
		ReadOnly = k_ReadBit,
		ReadWrite = k_ReadBit | k_WriteBit,
		ReadExecute = k_ReadBit | k_ExecuteBit
	};


	//
	// Private data
private:
	Type m_Type;
	Permissions m_Permissions;
	VFSPath m_VirtualPath;
	VFSPath m_RealMount;
};

///////////////////////////////////////////////////////////////////////////////
}}
